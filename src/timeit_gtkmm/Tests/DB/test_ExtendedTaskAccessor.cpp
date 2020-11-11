#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_ExtendedTaskAccessor.h"
#include "TempDB.h"
#include "ExtendedTaskAccessor.h"
#include "dbexception.h"

using namespace DB;

namespace Test
{

void ExtendedTaskAccessor_getExtendedTask()
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->newTime(taskId2, 0, 1000);

	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQUAL("Test", task.name());
	ASSERT_EQUAL(2000, task.totalTime());

	ExtendedTask task2 = taskAccessor->getExtendedTask(taskId2)->at(0);
	ASSERT_EQUAL("Test2", task2.name());
	ASSERT_EQUAL(1000, task2.totalTime());
}

void ExtendedTaskAccessor_getExtendedTasks()
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();

	int64_t taskId = taskAccessor->newTask("Test", 0);

	std::shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getExtendedTasks();
	ASSERT_EQUAL(1, tasks->size());

	taskAccessor->newTask("NextTask", 0);
	tasks = taskAccessor->getExtendedTasks();
	ASSERT_EQUAL(2, tasks->size());

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);
	timeAccessor->newTime(taskId2, 0, 1000);

	tasks = taskAccessor->getExtendedTasks();
	ExtendedTask &task = tasks->at(0);
	ASSERT_EQUAL("Test", task.name());
	ASSERT_EQUAL(1000, task.totalTime());
}

void ExtendedTaskAccessor_getRunningTasks()
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId2, 0, 1000);
	timeAccessor->setRunning(timeId, true);

	std::shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getRunningTasks();
	ASSERT_EQUAL(1, tasks->size());
	ExtendedTask task = tasks->at(0);
	ASSERT_EQUAL("Test2", task.name());

	tasks = taskAccessor->getExtendedTasks();
	auto task2 = tasks->at(0);
	ASSERT_EQUAL(1000, task2.totalTime());
}

void ExtendedTaskAccessor_testTotalTime()
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId2, 0, 1000);

	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQUAL(1000, task.totalTime());
}

void ExtendedTaskAccessor_setExpandedTasks()
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	ExtendedTask task = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQUAL(false, task.expanded());
	taskAccessor->setTaskExpanded(taskId, true);
	auto task2 = taskAccessor->getExtendedTask(taskId)->at(0);
	ASSERT_EQUAL(true, task2.expanded());
}

void ExtendedTaskAccessor_testTimeReporting()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t parentId = taskAccessor->newTask("test", 0);
	const int64_t taskId = taskAccessor->newTask("test", parentId);
	timeAccessor->newTime(taskId, 4000, 5000);

	std::shared_ptr<std::vector<ExtendedTask>> tasks = taskAccessor->getExtendedTask(parentId, 0, 0);
	ExtendedTask task = tasks->at(0);
	int result = task.totalTime();
	ASSERT_EQUALM("Test1", 1000, result);

	tasks = taskAccessor->getExtendedTask(taskId, 0, 0);
	auto task2 = tasks->at(0);
	result = task2.totalTime();
	ASSERT_EQUALM("Test2", 1000, result);

	tasks = taskAccessor->getExtendedTasks(0, 0, 0);
	auto task3 = tasks->at(0);
	result = task.totalTime();
	ASSERT_EQUALM("Test3", 1000, result);

	tasks = taskAccessor->getExtendedTasks(parentId, 0, 0);
	auto task4 = tasks->at(0);
	result = task4.totalTime();
	ASSERT_EQUALM("Test4", 1000, result);

}

cute::suite make_suite_test_ExtendedTaskAccessor()
{
	cute::suite s;
	s.push_back(CUTE(ExtendedTaskAccessor_getExtendedTask));
	s.push_back(CUTE(ExtendedTaskAccessor_getExtendedTasks));
	s.push_back(CUTE(ExtendedTaskAccessor_setExpandedTasks));
	s.push_back(CUTE(ExtendedTaskAccessor_getRunningTasks));
	s.push_back(CUTE(ExtendedTaskAccessor_testTotalTime));
	s.push_back(CUTE(ExtendedTaskAccessor_setExpandedTasks));
	s.push_back(CUTE(ExtendedTaskAccessor_testTimeReporting));
	return s;
}

}

