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

#ifndef _TASK_ACCESSOR_HPP_
#define _TASK_ACCESSOR_HPP_

#include <vector>
#include <Task.h>
#include "CSQL.h"
#include <list>
#include <ITaskAccessor.h>
#include <string>

class TaskAccessor : public ITaskAccessor
{
public:
	friend class TimeAccessor;
	TaskAccessor(const std::string& dbname);
	~TaskAccessor();

	void attach(TaskAccessorObserver*);
	void detach(TaskAccessorObserver*);

	virtual std::vector<Task> getTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
	virtual std::vector<Task> getRunningTasks(int64_t parentID = 0);
	virtual Task getTask(int64_t taskID, time_t start = 0, time_t stop = 0 );
	virtual int64_t newTask(std::string name, int64_t parentID);
	virtual void setTaskExpanded(int64_t taskID, bool expanded);
	virtual void setTaskName(int64_t taskID, std::string name);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
private:
	void setTaskRunning(int64_t taskID, bool running);
	void taskUpdated(int64_t taskID);
	void taskRemoved(int64_t taskID);
	void taskAdded(int64_t taskID);
	void taskParentChanged(int64_t taskID);

	std::vector<Task> _getTasks(int64_t parentID = 0, bool onlyRunning = false, time_t start = 0, time_t stop = 0);
	int getTotalChildTime(int64_t id, time_t start = 0 , time_t stop = 0);
	DBAbstraction::CSQL db;
	std::list<TaskAccessorObserver*> observers;
	dbexception dbe;
};

#endif // _TASK_MANAGER_HPP_
