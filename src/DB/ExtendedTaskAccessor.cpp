#include <iostream>
#include <sstream>
#include "ExtendedTaskAccessor.h"

using namespace std;
using namespace DBAbstraction;
namespace DB
{

ExtendedTaskAccessor::ExtendedTaskAccessor(shared_ptr<CSQL>& op_db, std::shared_ptr<Notifier> notifier,
		std::shared_ptr<ITimeAccessor> timeAccessor) :
		TaskAccessor(op_db, notifier)
{
	this->timeAccessor = timeAccessor;
}

ExtendedTaskAccessor::~ExtendedTaskAccessor()
{
}


std::shared_ptr<vector<ExtendedTask>> ExtendedTaskAccessor::getExtendedTasks(int64_t parentID, time_t start,
		time_t stop)
{
	std::shared_ptr<vector<ExtendedTask>> tasks = _getExtendedTasks(0, parentID, false, start, stop);
	for (unsigned int i = 0; i < tasks->size(); i++)
	{
		ExtendedTask& task = tasks->at(i);
		int totalTime = timeAccessor->getTotalTimeWithChildren(task.getID(), start, stop);
		task.setTotalTime(totalTime);
	}
	return tasks;
}

std::shared_ptr<vector<ExtendedTask>> ExtendedTaskAccessor::getRunningTasks(int64_t parentID)
{
	std::shared_ptr<vector<ExtendedTask>> tasks = _getExtendedTasks(0, parentID, true);
	for (unsigned int i = 0; i < tasks->size(); i++)
	{
		ExtendedTask& task = tasks->at(i);
		int totalTime = task.getTime();
		totalTime += getTotalChildTime(task.getID());
		task.setTotalTime(totalTime);
	}
	return tasks;
}

int ExtendedTaskAccessor::getTotalChildTime(int64_t id, time_t start, time_t stop)
{
	std::shared_ptr<vector<ExtendedTask>> tasks = _getExtendedTasks(0, id, false, start, stop);
	int totalTime = 0;
	for (ExtendedTask task : *tasks)
	{
		totalTime += task.getTime();
		totalTime += getTotalChildTime(task.getID(), start, stop);
	}
	return totalTime;
}

std::shared_ptr<vector<ExtendedTask>> ExtendedTaskAccessor::_getExtendedTasks(int64_t taskID, int64_t parentID,
		bool onlyRunning, time_t start, time_t stop)
{
	shared_ptr<vector<ExtendedTask>> retVal = shared_ptr<vector<ExtendedTask>>(new vector<ExtendedTask>);
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

	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (std::vector<DataCell> row : *rows)
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
		int time = timeAccessor->getTime(id, start, stop);
		ExtendedTask task(id, parent, name, time, expanded, running);
		retVal->push_back(task);
	}
	return retVal;
}

std::shared_ptr<vector<ExtendedTask>> ExtendedTaskAccessor::getExtendedTask(int64_t taskID, time_t start, time_t stop,
		bool calculateTotalTime)
{
	std::shared_ptr<vector<ExtendedTask>> tasks = _getExtendedTasks(taskID, 0, false, start, stop);
	if (tasks->size() == 1)
	{
		if (calculateTotalTime)
		{
			ExtendedTask& task = tasks->at(0);
			int totalTime = task.getTime();
			totalTime += getTotalChildTime(taskID, start, stop);
			task.setTotalTime(totalTime);
		}
	}
	return tasks;
}

void ExtendedTaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
	stringstream statement;
	statement << "UPDATE tasks SET expanded = " << expanded;
	statement << " WHERE id=" << taskID;
	db->exe(statement.str());
}


}
