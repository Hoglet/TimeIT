/*
 * MockTaskAccessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockTaskAccessor.h"

namespace Test
{

MockTaskAccessor::MockTaskAccessor()
{
}

MockTaskAccessor::~MockTaskAccessor()
{
}

void MockTaskAccessor::attach(TaskAccessorObserver*)
{
}

void MockTaskAccessor::detach(TaskAccessorObserver*)
{
}


std::shared_ptr<std::vector<Task> > MockTaskAccessor::getTask(int64_t taskID, time_t start, time_t stop, bool calculateTotalTime)
{
	std::shared_ptr<std::vector<Task>> returnValue = std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	Task task(1, 0, "Hum", 0, 0);
	returnValue->push_back(task);
	return returnValue;
}

std::shared_ptr<std::vector<Task>> MockTaskAccessor::getTasks(int64_t parentID, time_t start, time_t stop )
{
	std::shared_ptr<std::vector<Task>> list=std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	return list;
}

std::shared_ptr<std::vector<Task>> MockTaskAccessor::getRunningTasks(int64_t parentID)
{
	std::shared_ptr<std::vector<Task>> list=std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	return list;
}

int64_t MockTaskAccessor::newTask(std::string name, int64_t parentID)
{
	return 1;
}

void MockTaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
}

void MockTaskAccessor::setTaskName(int64_t taskID, std::string name)
{
}

void MockTaskAccessor::setParentID(int64_t taskID, int parentID)
{
}

void MockTaskAccessor::removeTask(int64_t taskID)
{
}

} /* namespace Test */
