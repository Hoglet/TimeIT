#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/ExtendedTaskAccessor.h>
#include <libtimeit/exceptions/dbexception.h>

namespace test
{


using namespace libtimeit;
using namespace std;

TEST(ExtendedTaskAccessor, getExtendedTask)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->newTime(taskId2, 0, 1000);

	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQ("Test", task.name());
	ASSERT_EQ(2000, task.totalTime());

	ExtendedTask task2 = taskAccessor->getExtendedTask(taskId2)->at(0);
	ASSERT_EQ("Test2", task2.name());
	ASSERT_EQ(1000, task2.totalTime());
}

TEST(ExtendedTaskAccessor, getExtendedTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();

	int64_t taskId = taskAccessor->newTask("Test", 0);

	shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getExtendedTasks();
	ASSERT_EQ(1, tasks->size());

	taskAccessor->newTask("NextTask", 0);
	tasks = taskAccessor->getExtendedTasks();
	ASSERT_EQ(2, tasks->size());

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);
	timeAccessor->newTime(taskId2, 0, 1000);

	tasks = taskAccessor->getExtendedTasks();
	ExtendedTask &task = tasks->at(0);
	ASSERT_EQ("Test", task.name());
	ASSERT_EQ(1000, task.totalTime());
}

TEST(ExtendedTaskAccessor, getRunningTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId2, 0, 1000);
	timeAccessor->setRunning(timeId, true);

	shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getRunningTasks();
	ASSERT_EQ(1, tasks->size());
	ExtendedTask task = tasks->at(0);
	ASSERT_EQ("Test2", task.name());

	tasks = taskAccessor->getExtendedTasks();
	auto task2 = tasks->at(0);
	ASSERT_EQ(1000, task2.totalTime());
}

TEST(ExtendedTaskAccessor, testTotalTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId2, 0, 1000);

	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQ(1000, task.totalTime());
}

TEST(ExtendedTaskAccessor, setExpandedTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQ(false, task.expanded());
	taskAccessor->setTaskExpanded(taskId, true);
	auto task2 = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQ(true, task2.expanded());
}

TEST(ExtendedTaskAccessor, testTimeReporting)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t parentId = taskAccessor->newTask("test", 0);
	const int64_t taskId = taskAccessor->newTask("test", parentId);
	timeAccessor->newTime(taskId, 4000, 5000);

	shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getExtendedTask(parentId, 0, 0);
	ExtendedTask task = tasks->at(0);
	int result = task.totalTime();
	ASSERT_EQ(1000, result);

	tasks = taskAccessor->getExtendedTask(taskId, 0, 0);
	auto task2 = tasks->at(0);
	result = task2.totalTime();
	ASSERT_EQ(1000, result);

	tasks = taskAccessor->getExtendedTasks(0, 0, 0);
	auto task3 = tasks->at(0);
	result = task.totalTime();
	ASSERT_EQ(1000, result);

	tasks = taskAccessor->getExtendedTasks(parentId, 0, 0);
	auto task4 = tasks->at(0);
	result = task4.totalTime();
	ASSERT_EQ(1000, result);

}

}