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

std::shared_ptr<vector<Task>> TaskAccessor::getTasks(int64_t parentID, time_t start, time_t stop)
{
	std::shared_ptr<vector<Task>> tasks = _getTasks(0, parentID, false, start, stop);
	for (unsigned int i = 0; i < tasks->size();i++)
	{
		Task& task = tasks->at(i);
		int totalTime = task.getTime();
		totalTime += getTotalChildTime(task.getID(), start, stop);
		task.setTotalTime(totalTime);
	}
	return tasks;
}

std::shared_ptr<vector<Task>> TaskAccessor::getRunningTasks(int64_t parentID)
{
	std::shared_ptr<vector<Task>> tasks = _getTasks(0, parentID, true);
	for (unsigned int i=0; i < tasks->size(); i++)
	{
		Task& task = tasks->at(i);
		int totalTime = task.getTime();
		totalTime += getTotalChildTime(task.getID());
		task.setTotalTime(totalTime);
	}
	return tasks;
}

int TaskAccessor::getTotalChildTime(int64_t id, time_t start, time_t stop)
{
	std::shared_ptr<vector<Task>> tasks = _getTasks(0, id, false, start, stop);
	int totalTime = 0;
	for (Task task : *tasks)
	{
		totalTime += task.getTime();
		totalTime += getTotalChildTime(task.getID(), start, stop);
	}
	return totalTime;
}

std::shared_ptr<vector<Task>> TaskAccessor::_getTasks(int64_t taskID, int64_t parentID, bool onlyRunning, time_t start,
		time_t stop)
{
	shared_ptr<vector<Task>> retVal=shared_ptr<vector<Task>>(new vector<Task>);
	int time = 0;
	stringstream statement;

	statement << "SELECT id, parent, name, expanded, running "
			"  FROM "
			"    tasks"
			"  WHERE deleted='false'";

	if (taskID > 0)
	{
		statement << " AND id=" << taskID;
	} else if (onlyRunning)
	{
		statement << " AND running=1";
	} else
	{
		statement << " AND parent=" << parentID;
	}

	db.exe(statement.str());
	for (std::vector<DataCell> row: db.rows)
	{
		int id = row[0].getInt();
		int parent = row[1].getInt();
		string name = row[2].getString();
		bool expanded = row[3].getBool();
		bool running = row[4].getBool();
		time = timeAccessor->getTime(id, start, stop);
		Task task(id, parent, name, time, expanded, running);
		retVal->push_back(task);
	}
	return retVal;
}

std::shared_ptr<vector<Task>> TaskAccessor::getTask(int64_t taskID, time_t start, time_t stop, bool calculateTotalTime)
{
	std::shared_ptr<vector<Task>> tasks = _getTasks(taskID, 0, false, start, stop);
	if (tasks->size() == 1)
	{
		if (calculateTotalTime)
		{
			Task& task=tasks->at(0);
			int totalTime = task.getTime();
			totalTime += getTotalChildTime(taskID, start, stop);
			task.setTotalTime(totalTime);
		}
	}
	return tasks;
}

int64_t TaskAccessor::newTask(std::string name, int64_t parentID)
{
	int64_t id = 0;
	stringstream statement;
	if (parentID < 0)
	{
		statement << "parenID is <0" << " in " << __FILE__ << ":" << __LINE__ << endl;
		dbe.setMessage(statement.str());
		cerr << statement.str() << endl;
		throw dbe;
	}
	if (parentID > 0)
	{
		std::shared_ptr<std::vector<Task>> tasks=getTask(parentID);
		if(tasks->size()<1)
		{
			statement << "parent did not exist when creating new task" << " in " << __FILE__ << ":" << __LINE__ << endl;
			dbe.setMessage(statement.str());
			cerr << statement.str() << endl;
			throw dbe;
		}
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
	std::shared_ptr<std::vector<Task>> tasks = getTask(parentID);
	if(tasks->size()>0)
	{
		stringstream statement;
		statement << "UPDATE tasks SET parent = " << parentID;
		statement << " WHERE id=" << taskID;
		db.exe(statement.str());
		notifier->taskParentChanged(taskID);
	}
	else
	{
		stringstream statement;
		statement << "parent did not exist when changing parent on task" << " in " << __FILE__ << ":" << __LINE__ << endl;
		dbe.setMessage(statement.str());
		throw dbe;
	}
}

void TaskAccessor::removeTask(int64_t taskID)
{
	//ENHANCEMENT Column running should be taken from time into v_tasks. (Avoid duplicate data)
	shared_ptr<vector<Task>> tasks = getTask(taskID);
	if(tasks->size()>0)
	{
		stringstream statement;
		statement << "UPDATE tasks SET deleted = " << true;
		statement << " WHERE id=" << taskID;
		db.exe(statement.str());
		notifier->taskRemoved(taskID);
	}
}

TaskAccessor::TaskAccessor(const std::string& dbname, std::shared_ptr<Notifier> notifier,
		std::shared_ptr<ITimeAccessor> timeAccessor) :
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
