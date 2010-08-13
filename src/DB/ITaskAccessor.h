/*
 * ITaskAccessor.h
 *
 *  Created on: Mar 7, 2009
 *      Author: hoglet
 */

#ifndef ITASKACCESSOR_H_
#define ITASKACCESSOR_H_

#include <Task.h>
#include <vector>
#include <stdint.h>

class TaskAccessorObserver
{
public:
	virtual void on_taskAdded(const Task&)  = 0;
	virtual void on_taskUpdated(const Task&) = 0;
	virtual void on_taskRemoved(int64_t) = 0;
	virtual void on_taskParentChanged(const Task&) {};
	virtual void on_taskAutotrackChanged(int64_t taskID, bool autotrack) {};
//	virtual void on_taskAutotrackChanged(int64_t taskID, bool autotrackEnabled) {};
};

class ITaskAccessor
{
public:
	virtual void attach(TaskAccessorObserver*) = 0;
	virtual void detach(TaskAccessorObserver*) = 0;

	virtual Task getTask(int64_t taskID ) = 0;
	virtual std::vector<Task> getTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0) = 0;
	virtual std::vector<Task> getRunningTasks(int64_t parentID = 0) = 0;
	virtual int64_t newTask(std::string name, int64_t parentID) = 0;
	virtual void setTaskExpanded(int64_t taskID, bool expanded) = 0;
	virtual void setTaskName(int64_t taskID, std::string name) = 0;
	virtual void setParentID(int64_t taskID, int parentID) = 0;
	virtual void removeTask(int64_t taskID) = 0;
	virtual void setAutotrack(int64_t taskID, bool autotrack) = 0;

};

#endif /* ITASKACCESSOR_H_ */
