#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/ExtendedTaskAccessor.h>

namespace test
{
using namespace libtimeit;

ExtendedTask getTask(time_t start = 0, time_t stop = 0)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	ExtendedTaskAccessor taskAccessor(tempdb);
	TimeAccessor timeAccessor(tempdb);
	auto taskID = taskAccessor.newTask("task", 0);
	auto subTaskID = taskAccessor.newTask("subtask", taskID);
	timeAccessor.newTime(taskID, 100, 200);
	timeAccessor.newTime(subTaskID, 150, 200);
	return taskAccessor.getExtendedTask(taskID, start, stop).at(0);
}

TEST(TimeManagement, tasksTimeIs100)
{
	ExtendedTask task = getTask();
	ASSERT_EQ(100, task.time());
}

TEST(TimeManagement, tasksTotalTimeIs150)
{
	ExtendedTask task = getTask();
	ASSERT_EQ(150, task.totalTime());
}

TEST(TimeManagement, staggerTest1RecordedTimePassesEnd)
{
	ExtendedTask task = getTask(0, 150);
	ASSERT_EQ(50, task.time());
}

TEST(TimeManagement, staggerTest2RecordedTimeStartsBeforeStart)
{
	ExtendedTask task = getTask(150, 300);
	ASSERT_EQ(50, task.time());
}

TEST(TimeManagement, staggerTest3RecordedTotalTimePassesEnd)
{
	ExtendedTask task = getTask(0, 150);
	ASSERT_EQ(50, task.totalTime());
}

TEST(TimeManagement, staggerTest3RecordedTotalTimeStartsBeforeStart)
{
	ExtendedTask task = getTask(150, 300);
	ASSERT_EQ(100, task.totalTime());
}

}