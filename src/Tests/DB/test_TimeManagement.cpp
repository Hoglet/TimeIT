#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TimeManagement.h"
#include "TempDB.h"

using namespace DB;

namespace Test
{

ExtendedTask getTask(time_t start = 0, time_t stop = 0)
{
	TempDB tempdb;
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t taskID = taskAccessor->newTask("task", 0);
	int64_t subTaskID = taskAccessor->newTask("subtask",taskID);
	timeAccessor->newTime(taskID, 100, 200);
	timeAccessor->newTime(subTaskID, 150, 200);
	return taskAccessor->getExtendedTask(taskID, start, stop)->at(0);
}

void TimeManagement_tasksTimeIs100()
{
	ExtendedTask task = getTask();
	ASSERT_EQUAL(100, task.getTime());
}

void TimeManagement_tasksTotalTimeIs150()
{
	ExtendedTask task = getTask();
	ASSERT_EQUAL(150, task.getTotalTime());
}

void TimeManagement_staggerTest1RecordedTimePassesEnd()
{
	ExtendedTask task = getTask(0,150);
	ASSERT_EQUAL(50, task.getTime());
}

void TimeManagement_staggerTest2RecordedTimeStartsBeforeStart()
{
	ExtendedTask task = getTask(150,300);
	ASSERT_EQUAL(50, task.getTime());
}

void TimeManagement_staggerTest3RecordedTotalTimePassesEnd()
{
	ExtendedTask task = getTask(0,150);
	ASSERT_EQUAL(50, task.getTotalTime());
}
void TimeManagement_staggerTest3RecordedTotalTimeStartsBeforeStart()
{
	ExtendedTask task = getTask(150,300);
	ASSERT_EQUAL(100, task.getTotalTime());
}

cute::suite make_suite_test_TimeManagement()
{
	cute::suite s;
	s.push_back(CUTE(TimeManagement_tasksTimeIs100));
	s.push_back(CUTE(TimeManagement_tasksTotalTimeIs150));
	s.push_back(CUTE(TimeManagement_staggerTest1RecordedTimePassesEnd));
	s.push_back(CUTE(TimeManagement_staggerTest2RecordedTimeStartsBeforeStart));
	s.push_back(CUTE(TimeManagement_staggerTest3RecordedTotalTimeStartsBeforeStart));
	s.push_back(CUTE(TimeManagement_staggerTest3RecordedTotalTimePassesEnd));
	return s;
}

}
