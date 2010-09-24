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

#ifndef _TASK_HPP_
#define _TASK_HPP_
#include <string>


 class Task
 {
	 public:
		 std::string getName() const;
		 void setName(std::string newName);
		 int getID() const;
		 bool getExpanded() const;
		 void setExpanded(bool);
		 int getParentID() const;
		 void setParent(int);
		 void setParentID();
		 int getTotalTime() const;
		 void setTotalTime(int);
		 int getTime() const;
		 void setTime(int) ;
		 bool getRunning() const;
		 Task(int ID, int parentID, std::string name, int time=0, bool expanded = false, bool running = false, int totalTime=0);
	 protected:
		 std::string name;
		 int ID;
		 bool expanded;
		 bool running;
		 int parentID;
		 int totalTime;
		 int time;
	 private:
		 Task() {};
 };

#endif // _TASK_HPP_

