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

Extended_task_accessor::~Extended_task_accessor()
{
}


vector<Extended_task> Extended_task_accessor::by_parent_ID(int64_t parentID, time_t start,
														   time_t stop)
{
	vector<Extended_task> tasks = _getExtendedTasks(0, parentID, false, start, stop);
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		Extended_task& task = tasks.at(i);
		int totalTime = time_accessor.total_cumulative_time(task.id, start, stop);
		task.total_time_=totalTime;
	}
	return tasks;
}


Duration Extended_task_accessor::getTotalChildTime(int64_t id, time_t start, time_t stop)
{
	auto tasks = _getExtendedTasks(0, id, false, start, stop);
	int totalTime = 0;
	for (auto task : tasks)
	{
		totalTime += task.time;
		totalTime += getTotalChildTime(task.id, start, stop);
	}
	return totalTime;
}

vector<Extended_task> Extended_task_accessor::_getExtendedTasks(int64_t taskID, int64_t parentID,
																bool onlyRunning, time_t start, time_t stop)
{
	vector<Extended_task> retVal;
	stringstream statement;

	statement << "SELECT id, parent, name, expanded, running "
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
		Extended_task task(id, parent, name, time, expanded, running);
		retVal.push_back(task);
	}
	return retVal;
}

optional<Extended_task> Extended_task_accessor::by_ID(int64_t taskID, time_t start, time_t stop)
{
	vector<Extended_task> tasks = _getExtendedTasks(taskID, 0, false, start, stop);
	if (tasks.size() == 1)
	{
		Extended_task& task = tasks.at(0);
		int totalTime = task.time;
		totalTime += getTotalChildTime(taskID, start, stop);
		task.total_time_=totalTime;
		return task;
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
