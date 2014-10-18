#include "MockTaskAccessor.h"

using namespace DB;
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

std::shared_ptr<Task> MockTaskAccessor::getTask(int64_t taskID)
{
	std::shared_ptr<Task> returnValue;
	return returnValue;
}

std::shared_ptr<std::vector<Task>> MockTaskAccessor::getTasks(int64_t parentID)
{
	std::shared_ptr<std::vector<Task>> returnValue = std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	return returnValue;
}

std::shared_ptr<std::vector<Task>> MockTaskAccessor::getTasksChangedSince(time_t timestamp)
{
	std::shared_ptr<std::vector<Task>> returnValue = std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	return returnValue;
}

int64_t MockTaskAccessor::newTask(const Task& task)
{
	return 0;
}

bool MockTaskAccessor::updateTask(const Task& task)
{
	return false;
}

std::shared_ptr<std::vector<ExtendedTask> > MockTaskAccessor::getExtendedTask(int64_t taskID, time_t start, time_t stop,
		bool calculateTotalTime)
{
	std::shared_ptr<std::vector<ExtendedTask>> returnValue = std::shared_ptr<std::vector<ExtendedTask>>(
			new std::vector<ExtendedTask>);
	ExtendedTask task(1, 0, "Hum", 0, 0);
	returnValue->push_back(task);
	return returnValue;
}

std::shared_ptr<std::vector<ExtendedTask>> MockTaskAccessor::getExtendedTasks(int64_t parentID, time_t start,
		time_t stop)
{
	std::shared_ptr<std::vector<ExtendedTask>> list = std::shared_ptr<std::vector<ExtendedTask>>(
			new std::vector<ExtendedTask>);
	return list;
}

std::shared_ptr<std::vector<ExtendedTask>> MockTaskAccessor::getRunningTasks(int64_t parentID)
{
	std::shared_ptr<std::vector<ExtendedTask>> list = std::shared_ptr<std::vector<ExtendedTask>>(
			new std::vector<ExtendedTask>);
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

int64_t MockTaskAccessor::uuidToId(std::string uuid)
{
	return 0;
}

void MockTaskAccessor::enableNotifications(bool)
{

}


} /* namespace Test */
