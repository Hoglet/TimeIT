#include "MockTaskAccessor.h"

using namespace libtimeit;
using namespace std;

namespace Test
{

MockTaskAccessor::MockTaskAccessor()
{
}

MockTaskAccessor::~MockTaskAccessor()
{
}

void MockTaskAccessor::attach(EventObserver*)
{
}

void MockTaskAccessor::detach(EventObserver*)
{
}

shared_ptr<Task> MockTaskAccessor::getTask(int64_t taskID)
{
	shared_ptr<Task> returnValue;
	return returnValue;
}

shared_ptr<vector<Task>> MockTaskAccessor::getTasks(int64_t parentID)
{
	shared_ptr<vector<Task>> returnValue = shared_ptr<vector<Task>>(new vector<Task>);
	return returnValue;
}

std::shared_ptr<std::vector<Task>> MockTaskAccessor::getTasksChangedSince(time_t timestamp)
{
	std::shared_ptr<std::vector<Task>> returnValue = shared_ptr<vector<Task>>(new vector<Task>);
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

std::shared_ptr<vector<ExtendedTask> > MockTaskAccessor::getExtendedTask(int64_t taskID, time_t start, time_t stop,
		bool calculateTotalTime)
{
	std::shared_ptr<vector<ExtendedTask>> returnValue = shared_ptr<vector<ExtendedTask>>(
			new vector<ExtendedTask>);
	ExtendedTask task(1, 0, "Hum", 0, 0);
	returnValue->push_back(task);
	return returnValue;
}

std::shared_ptr<vector<ExtendedTask>> MockTaskAccessor::getExtendedTasks(int64_t parentID, time_t start,
		time_t stop)
{
	shared_ptr<std::vector<ExtendedTask>> list = shared_ptr<vector<ExtendedTask>>(
			new vector<ExtendedTask>);
	return list;
}

shared_ptr<vector<ExtendedTask>> MockTaskAccessor::getRunningTasks(int64_t parentID)
{
	shared_ptr<std::vector<ExtendedTask>> list = shared_ptr<vector<ExtendedTask>>(
			new vector<ExtendedTask>);
	return list;
}

int64_t MockTaskAccessor::newTask(string name, int64_t parentID)
{
	return 1;
}

void MockTaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
}

void MockTaskAccessor::setTaskName(int64_t taskID, string name)
{
}

void MockTaskAccessor::setParentID(int64_t taskID, int parentID)
{
}

void MockTaskAccessor::removeTask(int64_t taskID)
{
}

int64_t MockTaskAccessor::uuidToId(UUID uuid)
{
	return 0;
}

void MockTaskAccessor::enableNotifications(bool)
{

}

} /* namespace Test */
