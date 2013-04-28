#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TaskAccessor.h"
#include "TempDB.h"
#include "ITaskAccessor.h"
#include "dbexception.h"

namespace Test
{

void testNewTask_inputValidation()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	ASSERT_THROWS(taskAccessor->newTask("Test", -1), dbexception);
	ASSERT_THROWS(taskAccessor->newTask("Test", 1), dbexception);
}

void testGetTask()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId2, 0, 1000);

	Task task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Test", task.getName());
	ASSERT_EQUAL(1000, task.getTotalTime());
}

void testGetTasks()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();

	int64_t taskId = taskAccessor->newTask("Test", 0);

	std::vector<Task> tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(1, tasks.size());

	taskAccessor->newTask("NextTask", 0);
	tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(2, tasks.size());

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);
	timeAccessor->newTime(taskId2, 0, 1000);

	tasks = taskAccessor->getTasks();
	Task task = tasks[0];
	ASSERT_EQUAL("Test", task.getName());
	ASSERT_EQUAL(1000, task.getTotalTime());
}

void testGetRunningTasks()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);

	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId2, 0, 1000);
	timeAccessor->setRunning(timeId, true);

	std::vector<Task> tasks = taskAccessor->getRunningTasks();
	ASSERT_EQUAL(1, tasks.size());
	Task task = tasks[0];
	ASSERT_EQUAL("Test2", task.getName());

	tasks = taskAccessor->getTasks();
	task = tasks[0];
	ASSERT_EQUAL(1000, task.getTotalTime());
}

void testTotalTime()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", taskId);

	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId2, 0, 1000);

	Task task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL(1000, task.getTotalTime());
}

void testSetExpandedTasks()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	Task task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL(false, task.getExpanded());
	taskAccessor->setTaskExpanded(taskId, true);
	task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL(true, task.getExpanded());
}

void testSetTaskName()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	Task task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Test", task.getName());
	taskAccessor->setTaskName(taskId, "Tjohopp");
	task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Tjohopp", task.getName());
}

void testSetParentID()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId1 = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", 0);
	Task task = taskAccessor->getTask(taskId1);
	ASSERT_EQUAL(0, task.getParentID());
	taskAccessor->setParentID(taskId1, taskId2);
	task = taskAccessor->getTask(taskId1);
	ASSERT_EQUAL(taskId2, task.getParentID());
}

void testSetParentID_inputValidation()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	Task task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL(0, task.getParentID());
	ASSERT_THROWS(taskAccessor->setParentID(taskId, taskId + 1), dbexception);
}

void testRemove()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	std::vector<Task> tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(1, tasks.size());
	taskAccessor->removeTask(taskId);
	tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(0, tasks.size());
}

cute::suite make_suite_test_TaskAccessor()
{
	cute::suite s;
	s.push_back(CUTE(testGetTask));
	s.push_back(CUTE(testGetTasks));
	s.push_back(CUTE(testSetTaskName));
	s.push_back(CUTE(testSetExpandedTasks));
	s.push_back(CUTE(testSetParentID));
	s.push_back(CUTE(testSetParentID_inputValidation));
	s.push_back(CUTE(testRemove));
	s.push_back(CUTE(testNewTask_inputValidation));
	s.push_back(CUTE(testGetRunningTasks));
	return s;
}

}

