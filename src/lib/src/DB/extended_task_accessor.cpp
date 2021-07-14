#include <iostream>
#include <sstream>
#include <libtimeit/db/extended_task_accessor.h>

using namespace std;
namespace libtimeit
{

extended_task_accessor::extended_task_accessor(database& db )
		:
		task_accessor(db),
		times(db)
{
}




vector<extended_task> extended_task_accessor::by_parent_id(int64_t parentID, time_t start,
														   time_t stop)
{
	return get_extended_tasks(0, parentID, false, start, stop);
}


Duration extended_task_accessor::get_total_child_time(int64_t id, time_t start, time_t stop)
{
	auto children = get_extended_tasks(0, id, false, start, stop);
	int total_time = 0;
	for (auto child : children)
	{
		total_time += child.time;
		total_time += get_total_child_time(child.id, start, stop);
	}
	return total_time;
}

vector<extended_task> extended_task_accessor::get_extended_tasks(int64_t taskID, int64_t parentID,
																 bool onlyRunning, time_t start, time_t stop)
{
	vector<extended_task> return_value;
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

	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
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
		int time = times.duration_time(id, start, stop);
		int total_time = times.total_cumulative_time(id, start, stop);
		auto opt_uuid=UUID::from_string(row[5].text());

		if(opt_uuid.has_value())
		{
			return_value.emplace_back(
					id,
					opt_uuid.value(),
					parent,
					name,
					time,
					expanded,
					running,
					total_time);
		}
	}
	return return_value;
}

optional<extended_task> extended_task_accessor::by_id(int64_t taskID, time_t start, time_t stop)
{
	vector<extended_task> tasks = get_extended_tasks(taskID, 0, false, start, stop);
	if (tasks.size() == 1)
	{
		return tasks.at(0);
	}
	return {};
}


void extended_task_accessor::drop_views(database& db)
{
	db.execute("DROP VIEW IF EXISTS v_running");
	db.execute("DROP VIEW IF EXISTS v_tasks");
}

void extended_task_accessor::create_views(database& db)
{
	db.execute(
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
	db.execute(
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

void extended_task_accessor::upgrade(database& /*database*/)
{
	//noop
}

void extended_task_accessor::setup(database& db)
{
	extended_task_accessor::drop_views(db);
	extended_task_accessor::upgrade(db);
	extended_task_accessor::create_views(db);
}


}
