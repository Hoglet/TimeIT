#include <iostream>
#include <sstream>
#include <libtimeit/db/extended_task_accessor.h>

using namespace std;
namespace libtimeit
{

Extended_task_accessor::Extended_task_accessor(Database& database )
		:
		Task_accessor(database),
		time_accessor( database)
{
}




vector<Extended_task> Extended_task_accessor::by_parent_id(int64_t parentID, time_t start,
														   time_t stop)
{
	vector<Extended_task> tasks = get_extended_tasks(0, parentID, false, start, stop);
	return tasks;
}


Duration Extended_task_accessor::get_total_child_time(int64_t id, time_t start, time_t stop)
{
	auto tasks = get_extended_tasks(0, id, false, start, stop);
	int total_time = 0;
	for (auto task : tasks)
	{
		total_time += task.time;
		total_time += get_total_child_time(task.id, start, stop);
	}
	return total_time;
}

vector<Extended_task> Extended_task_accessor::get_extended_tasks(int64_t taskID, int64_t parentID,
																 bool onlyRunning, time_t start, time_t stop)
{
	vector<Extended_task> return_value;
	stringstream statement;

	statement << "SELECT id, parent, name, expanded, running, uuid"
			"  FROM "
			"    v_tasks"
			"  WHERE deleted=0";

	if (taskID > 0)
	{
		statement << " AND id=" << taskID;
	}
	else if (onlyRunning)
	{
		statement << " AND running=1";
	}
	else
	{
		if (parentID > 0)
		{
			statement << " AND parent=" << parentID;
		}
		else
		{
			statement << " AND parent IS NULL ";
		}
	}

	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
	{
		int id = (int)row[0].integer();
		int parent = 0;
		if (row[1].has_value())
		{
			parent = (int)row[1].integer();
		}
		string name = row[2].text();
		bool expanded = row[3].boolean();
		bool running = row[4].boolean();
		int time = time_accessor.duration_time(id, start, stop);
		int total_time = time;
		total_time = time_accessor.total_cumulative_time(id, start, stop);
		auto opt_uuid=UUID::from_string(row[5].text());

		if(opt_uuid.has_value())
		{
			Extended_task task(
					id,
					opt_uuid.value(),
					parent,
					name,
					time,
					expanded,
					running,
					total_time);
			return_value.push_back(task);
		}
	}
	return return_value;
}

optional<Extended_task> Extended_task_accessor::by_id(int64_t taskID, time_t start, time_t stop)
{
	vector<Extended_task> tasks = get_extended_tasks(taskID, 0, false, start, stop);
	if (tasks.size() == 1)
	{
		return tasks.at(0);
	}
	return {};
}


void Extended_task_accessor::drop_views()
{
	database.execute("DROP VIEW IF EXISTS v_running");
	database.execute("DROP VIEW IF EXISTS v_tasks");
}

void Extended_task_accessor::create_views()
{
	database.execute(
			R"Query(
				CREATE VIEW
					v_running
				AS SELECT
					taskID,
					running
				FROM
				    times
				WHERE running ="1";
			)Query");
	database.execute(
			R"Query(
				CREATE VIEW
					v_tasks
				AS SELECT
					tasks.*,
					IFNULL(v_running.running,0) as running
				FROM
					tasks
				LEFT JOIN
					v_running
				ON
					tasks.id=v_running.taskId
				)Query");

}


}
