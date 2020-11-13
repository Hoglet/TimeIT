#include "gtest/gtest.h"
#include "TempDB.h"

using namespace DB;

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

TEST( TimeManagement, tasksTimeIs100)
{
	ExtendedTask task = getTask();
	ASSERT_EQ(100, task.time());
}

TEST( TimeManagement, tasksTotalTimeIs150)
{
	ExtendedTask task = getTask();
	ASSERT_EQ(150, task.totalTime());
}

TEST( TimeManagement, staggerTest1RecordedTimePassesEnd)
{
	ExtendedTask task = getTask(0,150);
	ASSERT_EQ(50, task.time());
}

TEST( TimeManagement, staggerTest2RecordedTimeStartsBeforeStart)
{
	ExtendedTask task = getTask(150,300);
	ASSERT_EQ(50, task.time());
}

TEST( TimeManagement, staggerTest3RecordedTotalTimePassesEnd)
{
	ExtendedTask task = getTask(0,150);
	ASSERT_EQ(50, task.totalTime());
}

TEST( TimeManagement, staggerTest3RecordedTotalTimeStartsBeforeStart)
{
	ExtendedTask task = getTask(150,300);
	ASSERT_EQ(100, task.totalTime());
}

