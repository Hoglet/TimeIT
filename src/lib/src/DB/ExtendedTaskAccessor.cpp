#include <iostream>
#include <sstream>
#include <libtimeit/db/ExtendedTaskAccessor.h>

using namespace std;
namespace libtimeit
{

ExtendedTaskAccessor::ExtendedTaskAccessor(	Database& database )
		:
		TaskAccessor(database),
		time_accessor( database)
{
}

ExtendedTaskAccessor::~ExtendedTaskAccessor()
{
}


vector<ExtendedTask> ExtendedTaskAccessor::getExtendedTasks(int64_t parentID, time_t start,
		time_t stop)
{
	vector<ExtendedTask> tasks = _getExtendedTasks(0, parentID, false, start, stop);
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		ExtendedTask& task = tasks.at(i);
		int totalTime = time_accessor.getTotalTimeWithChildren(task.ID(), start, stop);
		task.totalTime_=totalTime;
	}
	return tasks;
}

vector<ExtendedTask> ExtendedTaskAccessor::getRunningTasks(int64_t parentID)
{
	vector<ExtendedTask> tasks = _getExtendedTasks(0, parentID, true);
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		ExtendedTask& task = tasks.at(i);
		int totalTime = task.time();
		totalTime += getTotalChildTime(task.ID());
		task.totalTime_=totalTime;
	}
	return tasks;
}

int ExtendedTaskAccessor::getTotalChildTime(int64_t id, time_t start, time_t stop)
{
	vector<ExtendedTask> tasks = _getExtendedTasks(0, id, false, start, stop);
	int totalTime = 0;
	for (ExtendedTask task : tasks)
	{
		totalTime += task.time();
		totalTime += getTotalChildTime(task.ID(), start, stop);
	}
	return totalTime;
}

vector<ExtendedTask> ExtendedTaskAccessor::_getExtendedTasks(int64_t taskID, int64_t parentID,
		bool onlyRunning, time_t start, time_t stop)
{
	vector<ExtendedTask> retVal;
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

	QueryResult rows = database.exe(statement.str());
	for (vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		int parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool expanded = row[3].getBool();
		bool running = row[4].getBool();
		int time = time_accessor.getTime(id, start, stop);
		ExtendedTask task(id, parent, name, time, expanded, running);
		retVal.push_back(task);
	}
	return retVal;
}

vector<ExtendedTask> ExtendedTaskAccessor::getExtendedTask(int64_t taskID, time_t start, time_t stop,
		bool calculateTotalTime)
{
	vector<ExtendedTask> tasks = _getExtendedTasks(taskID, 0, false, start, stop);
	if (tasks.size() == 1)
	{
		if (calculateTotalTime)
		{
			ExtendedTask& task = tasks.at(0);
			int totalTime = task.time();
			totalTime += getTotalChildTime(taskID, start, stop);
			task.totalTime_=totalTime;
		}
	}
	return tasks;
}



}
