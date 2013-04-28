#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TimeManagement.h"
#include "TempDB.h"

namespace Test
{

Task getTask(time_t start = 0, time_t stop = 0)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t taskID = taskAccessor->newTask("task", 0);
	int64_t subTaskID = taskAccessor->newTask("subtask",taskID);
	timeAccessor->newTime(taskID, 100, 200);
	timeAccessor->newTime(subTaskID, 150, 200);
	return taskAccessor->getTask(taskID, start, stop);
}

void tasksTimeIs100()
{
	Task task = getTask();
	ASSERT_EQUAL(100, task.getTime());
}

void tasksTotalTimeIs150()
{
	Task task = getTask();
	ASSERT_EQUAL(150, task.getTotalTime());
}

void staggerTest1RecordedTimePassesEnd()
{
	Task task = getTask(0,150);
	ASSERT_EQUAL(50, task.getTime());
}

void staggerTest2RecordedTimeStartsBeforeStart()
{
	Task task = getTask(150,300);
	ASSERT_EQUAL(50, task.getTime());
}

void staggerTest3RecordedTotalTimePassesEnd()
{
	Task task = getTask(0,150);
	ASSERT_EQUAL(50, task.getTotalTime());
}
void staggerTest3RecordedTotalTimeStartsBeforeStart()
{
	Task task = getTask(150,300);
	ASSERT_EQUAL(100, task.getTotalTime());
}

cute::suite make_suite_test_TimeManagement()
{
	cute::suite s;
	s.push_back(CUTE(tasksTimeIs100));
	s.push_back(CUTE(tasksTotalTimeIs150));
	s.push_back(CUTE(staggerTest1RecordedTimePassesEnd));
	s.push_back(CUTE(staggerTest2RecordedTimeStartsBeforeStart));
	s.push_back(CUTE(staggerTest3RecordedTotalTimeStartsBeforeStart));
	s.push_back(CUTE(staggerTest3RecordedTotalTimePassesEnd));
	return s;
}

}
