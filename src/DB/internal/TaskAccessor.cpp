/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * TimeIT
 * Copyright (C) Kent Asplund 2008 <hoglet@solit.se>
 *
 * TimeIT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TimeIT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include "TaskAccessor.h"

using namespace std;
using namespace DBAbstraction;

vector<Task> TaskAccessor::getTasks(int64_t parentID, time_t start, time_t stop)
{
	vector<Task> tasks = _getTasks(0, parentID, false, start, stop);
	std::vector<Task>::iterator iter;
	for (iter = tasks.begin(); iter != tasks.end(); iter++)
	{
		int totalTime = iter->getTime();
		totalTime += getTotalChildTime(iter->getID(), start, stop);
		iter->setTotalTime(totalTime);
	}
	return tasks;
}

vector<Task> TaskAccessor::getRunningTasks(int64_t parentID)
{
	vector<Task> tasks = _getTasks(0, parentID, true);
	std::vector<Task>::iterator iter;
	for (iter = tasks.begin(); iter != tasks.end(); iter++)
	{
		int totalTime = iter->getTime();
		totalTime += getTotalChildTime(iter->getID());
		iter->setTotalTime(totalTime);
	}
	return tasks;
}

int TaskAccessor::getTotalChildTime(int64_t id, time_t start, time_t stop)
{
	vector<Task> tasks = _getTasks(0, id, false, start, stop);
	int totalTime = 0;
	std::vector<Task>::iterator iter;
	for (iter = tasks.begin(); iter != tasks.end(); iter++)
	{
		totalTime += iter->getTime();
		totalTime += getTotalChildTime(iter->getID(), start, stop);
	}
	return totalTime;
}

vector<Task> TaskAccessor::_getTasks(int64_t taskID, int64_t parentID, bool onlyRunning, time_t start, time_t stop)
{
	vector<Task> retVal;
	int time = 0;
	stringstream statement;

	statement << "SELECT id, parent, name, expanded, running "
			"  FROM "
			"    tasks"
			"  WHERE deleted='false'";

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
		statement << " AND parent=" << parentID;
	}

	db.exe(statement.str());
	for (unsigned int r = 0; r < db.rows.size(); r++)
	{
		vector<DataCell> row = db.rows[r];

		int id = row[0].getInt();
		int parent = row[1].getInt();
		string name = row[2].getString();
		bool expanded = row[3].getBool();
		bool running = row[4].getBool();
		time = timeAccessor->getTime(id, start, stop);
		Task task(id, parent, name, time, expanded, running);
		retVal.push_back(task);
	}
	return retVal;
}

Task TaskAccessor::getTask(int64_t taskID, time_t start, time_t stop, bool calculateTotalTime)
{
	vector<Task> tasks = _getTasks(taskID, 0, false, start, stop);
	if (tasks.size() == 1)
	{
		Task task = tasks[0];
		if (calculateTotalTime)
		{
			int totalTime = task.getTime();
			totalTime += getTotalChildTime(taskID, start, stop);
			task.setTotalTime(totalTime);
		}
		return task;
	}
	else
	{
		dbe.setReturnCode(0);
		dbe.setMessage("Task not found");
		throw dbe;
	}
}

int64_t TaskAccessor::newTask(std::string name, int64_t parentID)
{
	int64_t id = 0;
	stringstream statement;
	if (parentID < 0)
	{
		dbexception* e = new dbexception();
		statement << "parenID is <0" << " in " << __FILE__ << ":" << __LINE__ << endl;
		e->setMessage(statement.str());
		cerr << statement.str() << endl;
		throw e;
	}
	statement << "INSERT INTO tasks (name,parent) VALUES (\"" << name << "\", " << parentID << ")";

	db.exe(statement.str());
	id = db.getIDOfLastInsert();
	notifier->taskAdded(id);
	return id;
}
void TaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
	stringstream statement;
	statement << "UPDATE tasks SET expanded = " << expanded;
	statement << " WHERE id=" << taskID;
	db.exe(statement.str());
}

void TaskAccessor::setTaskName(int64_t taskID, std::string name)
{
	stringstream statement;
	statement << "UPDATE tasks SET name = \"" << name << "\" ";
	statement << " WHERE id=" << taskID;
	db.exe(statement.str());
	notifier->taskUpdated(taskID);
}

void TaskAccessor::setParentID(int64_t taskID, int parentID)
{
	stringstream statement;
	statement << "UPDATE tasks SET parent = " << parentID;
	statement << " WHERE id=" << taskID;
	db.exe(statement.str());
	notifier->taskParentChanged(taskID);

}

void TaskAccessor::removeTask(int64_t taskID)
{
	//ENHANCEMENT Column running should be taken from time into v_tasks. (Avoid duplicate data)
	Task task = getTask(taskID);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " WHERE id=" << taskID;
	db.exe(statement.str());
	notifier->taskRemoved(taskID);
}

void TaskAccessor::setTaskRunning(int64_t taskID, bool running)
{

}

TaskAccessor::TaskAccessor(const std::string& dbname, boost::shared_ptr<Notifier> notifier,
		boost::shared_ptr<ITimeAccessor> timeAccessor) :
		db(dbname)
{
	this->notifier = notifier;
	this->timeAccessor = timeAccessor;
	db.exe("UPDATE tasks SET running = 0");
	db.exe("UPDATE tasks SET parent = 0 WHERE parent < 0");
}
TaskAccessor::~TaskAccessor()
{
}

void TaskAccessor::attach(TaskAccessorObserver* observer)
{
	notifier->attach(observer);
}
void TaskAccessor::detach(TaskAccessorObserver* observer)
{
	notifier->detach(observer);
}
