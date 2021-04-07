#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/extended_task_accessor.h>

namespace test
{
using namespace libtimeit;

Extended_task getTask(time_t start = 0, time_t stop = 0)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Extended_task_accessor taskAccessor(tempdb);
	Time_accessor timeAccessor(tempdb);
	auto taskID = taskAccessor.create(Task("task", 0));
	auto subTaskID = taskAccessor.create(Task("subtask", taskID));
	timeAccessor.create( Time_entry( taskID, 100, 200 ) );
	timeAccessor.create( Time_entry( subTaskID, 150, 200 ) );
	return *taskAccessor.by_ID(taskID, start, stop);
}

TEST(TimeManagement, tasksTimeIs100)
{
	Extended_task task = getTask();
	ASSERT_EQ(100, task.time());
}

TEST(TimeManagement, tasksTotalTimeIs150)
{
	Extended_task task = getTask();
	ASSERT_EQ(150, task.total_time());
}

TEST(TimeManagement, staggerTest1RecordedTimePassesEnd)
{
	Extended_task task = getTask(0, 150);
	ASSERT_EQ(50, task.time());
}

TEST(TimeManagement, staggerTest2RecordedTimeStartsBeforeStart)
{
	Extended_task task = getTask(150, 300);
	ASSERT_EQ(50, task.time());
}

TEST(TimeManagement, staggerTest3RecordedTotalTimePassesEnd)
{
	Extended_task task = getTask(0, 150);
	ASSERT_EQ(50, task.total_time());
}

TEST(TimeManagement, staggerTest3RecordedTotalTimeStartsBeforeStart)
{
	Extended_task task = getTask(150, 300);
	ASSERT_EQ(100, task.total_time());
}

}