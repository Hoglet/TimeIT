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
	vector<Task> tasks = _getTasks(parentID, false, start, stop);
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
	vector<Task> tasks = _getTasks(parentID, true);
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
	vector<Task> tasks = _getTasks(id, false, start, stop);
	int totalTime = 0;
	std::vector<Task>::iterator iter;
	for (iter = tasks.begin(); iter != tasks.end(); iter++)
	{
		totalTime += iter->getTime();
		totalTime += getTotalChildTime(iter->getID(), start, stop);
	}
	return totalTime;
}

vector<Task> TaskAccessor::_getTasks(int64_t parentID, bool onlyRunning, time_t start, time_t stop)
{
	vector<Task> retVal;
	int totalTime = 0;
	stringstream statement;

	statement << "SELECT id, parent, name, expanded, running, time "
		"  FROM "
		"    ("
		"      SELECT"
		"        tasks.id as id, tasks.parent as parent,tasks.running as running,"
		"        tasks.name as name, tasks.expanded as expanded, times_time as time,"
		"        tasks.deleted as deleted "
		"      FROM"
		"        tasks "
		"      LEFT JOIN "
		"        ("
		"          SELECT"
		"            taskID as times_taskID, SUM(stop-start) AS times_time "
		"          FROM "
		"            times";
	if (stop > 0)
	{
		statement << ""
			"      WHERE"
			"        start >=" << start << "      AND"
			"        stop <= " << stop;
	}
	statement << ""
		"          GROUP BY "
		"            taskID"
		"        )"
		"      ON tasks.id=times_taskID"
		"    )"
		"WHERE deleted='false'";

	if (onlyRunning)
	{
		statement << " AND running=1";
	}
	else
	{
		statement << " AND parent=" << parentID;
	}
	try
	{
		db.exe(statement.str());
		for (unsigned int r = 0; r < db.rows.size(); r++)
		{
			vector<DataCell> row = db.rows[r];

			int id = row[0].getInt();
			int parent = row[1].getInt();
			string name = row[2].getString();
			bool expanded = row[3].getBool();
			bool running = row[4].getBool();

			if (row[5].isNull() == false)
			{
				totalTime = row[5].getInt();
			}
			else
			{
				totalTime = 0;
			}
			Task task(id, parent, name, totalTime, expanded, running);
			retVal.push_back(task);
		}
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
	}
	return retVal;
}

Task TaskAccessor::getTask(int64_t taskID, time_t start, time_t stop, bool calculateTotalTime)
{
	int id = 0;
	int parent = 0;
	string name;
	bool expanded = false;
	bool running = false;
	int time = 0;
	int totalTime = 0;
	stringstream statement;
	statement << "SELECT "
		"    id,parent,name,expanded,running,time "
		"  FROM "
		"    ("
		"      SELECT"
		"        tasks.id as id, tasks.parent as parent,tasks.running as running,"
		"        tasks.name as name, tasks.expanded as expanded, times_time as time,"
		"        tasks.deleted as deleted "
		"      FROM"
		"        tasks "
		"      LEFT JOIN "
		"        ("
		"          SELECT"
		"            taskID as times_taskID, SUM(stop-start) AS times_time "
		"          FROM "
		"            times";
	if (stop > 0)
	{
		statement << ""
			"      WHERE"
			"        start >=" << start << "      AND"
			"        stop <= " << stop;
	}

	statement << ""
		"          GROUP BY "
		"            taskID"
		"        )"
		"      ON tasks.id=times_taskID"
		"    )"
		"  WHERE id=" << taskID << "  AND deleted='false'";
	try
	{
		db.exe(statement.str());
		if (db.rows.size() > 0)
		{
			vector<DataCell> row = db.rows[0];

			id = row[0].getInt();
			parent = row[1].getInt();
			name = row[2].getString();
			expanded = row[3].getBool();
			running = row[4].getBool();
			if (row[5].isNull() == false)
			{
				time = row[5].getInt();
			}
			else
			{
				time = 0;
			}
			if(calculateTotalTime)
			{
				totalTime = time;
				totalTime += getTotalChildTime(id,start,stop);
			}
		}
		else
		{
			dbe.setReturnCode(0);
			dbe.setMessage("Task not found");
			throw dbe;
		}
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
	Task task(id, parent, name, time, expanded, running, totalTime);
	return task;

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
	try
	{
		db.exe(statement.str());
		id = db.getIDOfLastInsert();
		taskAdded(id);
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
	return id;
}
void TaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
	stringstream statement;
	statement << "UPDATE tasks SET expanded = " << expanded;
	statement << " WHERE id=" << taskID;
	try
	{
		db.exe(statement.str());
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
}


void TaskAccessor::setTaskName(int64_t taskID, std::string name)
{
	stringstream statement;
	statement << "UPDATE tasks SET name = \"" << name << "\" ";
	statement << " WHERE id=" << taskID;
	try
	{
		db.exe(statement.str());
		taskUpdated(taskID);
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
}

void TaskAccessor::setParentID(int64_t taskID, int parentID)
{
	stringstream statement;
	statement << "UPDATE tasks SET parent = " << parentID;
	statement << " WHERE id=" << taskID;
	try
	{
		db.exe(statement.str());
		taskParentChanged(taskID);
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}

}

void TaskAccessor::removeTask(int64_t taskID)
{
	//ENHANCEMENT Column running should be taken from time into v_tasks. (Avoid duplicate data)
	Task task = getTask(taskID);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " WHERE id=" << taskID;
	try
	{
		db.exe(statement.str());
		taskRemoved(taskID);
		int parentID = task.getParentID();
		if (parentID > 0)
		{
			taskUpdated(parentID);
		}
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
}

void TaskAccessor::setTaskRunning(int64_t taskID, bool running)
{
	//ENHANCEMENT Column running should be taken from time into v_tasks. (Avoid duplicate data)
	stringstream statement;
	statement << "UPDATE tasks SET running = " << running;
	statement << " WHERE id=" << taskID;
	try
	{
		db.exe(statement.str());
		taskUpdated(taskID);
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused :\n";
		cerr << e.what() << endl;
		throw e;
	}
}

TaskAccessor::TaskAccessor(const std::string& dbname) :
	db(dbname)
{
	try
	{
		db.exe("UPDATE tasks SET running = 0");
		db.exe("UPDATE tasks SET parent = 0 WHERE parent < 0");
	} catch (dbexception& e)
	{
		cerr << e.what() << endl;
		throw e;
	}
}
TaskAccessor::~TaskAccessor()
{
}

void TaskAccessor::taskUpdated(int64_t taskID)
{
	try
	{
		Task task = getTask(taskID);
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskUpdated(task);
		}
		if (task.getParentID() != 0)
		{
			taskUpdated(task.getParentID());
		}
	} catch (...)
	{
	}
}

void TaskAccessor::taskAdded(int64_t taskID)
{
	try
	{
		Task task = getTask(taskID);
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskAdded(task);
		}
		if (task.getParentID() != 0)
		{
			taskUpdated(task.getParentID());
		}
	} catch (...)
	{
	}
}

void TaskAccessor::taskParentChanged(int64_t taskID)
{
	try
	{
		Task task = getTask(taskID);
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskParentChanged(task);
		}
	} catch (...)
	{
	}
}

void TaskAccessor::taskRemoved(int64_t taskID)
{
	try
	{
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskRemoved(taskID);
		}
	} catch (...)
	{
	}
}

void TaskAccessor::attach(TaskAccessorObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}
void TaskAccessor::detach(TaskAccessorObserver* observer)
{
	if (observer)
	{
		observers.remove(observer);
	}
}
