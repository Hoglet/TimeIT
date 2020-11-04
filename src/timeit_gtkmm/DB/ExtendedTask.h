#ifndef _EXTENDEDTASK_HPP_
#define _EXTENDEDTASK_HPP_
#include "Task.h"

namespace DB
{
 class ExtendedTask: public Task
 {
	 public:
		 bool getExpanded() const;
		 void setExpanded(bool);
		 int getTotalTime() const;
		 void setTotalTime(int);
		 int getTime() const;
		 void setTime(int) ;
		 bool getRunning() const;
		 ExtendedTask(int ID, int parentID, std::string name, int time=0, bool expanded = false, bool running = false, int totalTime=0);
	 protected:
		 bool expanded;
		 bool running;
		 int totalTime;
		 int time;
	 private:
		 ExtendedTask();
 };
}
#endif // _EXTENDEDTASK_HPP_

