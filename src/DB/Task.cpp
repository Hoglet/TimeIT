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

#include "Task.h"

std::string Task::getName() const
{
	return name;
}
void Task::setName(std::string newName)
{
	name=newName;
}

int Task::getID() const
{
	return ID;
}
int Task::getParentID() const
{
	return parentID;
}
void Task::setParent(int op1)
{
	parentID=op1;
}
bool Task::getExpanded() const
{
	return expanded;
}
bool Task::getRunning() const
{
	return running;
}
void Task::setExpanded(bool op)
{
	expanded=op;
}
Task::Task(int op_ID, int op_parentID, std::string op_name, int op_time, bool op_expanded, bool op_running, int op_totalTime)
{
	ID = op_ID;
	parentID = op_parentID;
	name = op_name;
	totalTime = op_totalTime;
	expanded = op_expanded;
	running = op_running;
	time = op_time;
}

int Task::getTotalTime() const
{
	return totalTime;
}

void Task::setTotalTime(int op_totalTime)
{
	totalTime=op_totalTime;
}


int Task::getTime() const
{
	return time;
}

void Task::setTime(int op_time)
{
	time=op_time;
}
