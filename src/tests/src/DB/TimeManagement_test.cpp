#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/extended_task_accessor.h>

namespace test
{
using namespace libtimeit;

extended_task getTask(time_t start = 0, time_t stop = 0)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb);
	time_accessor timeAccessor(tempdb);
	auto taskID = taskAccessor.create(task("task", 0));
	auto subTaskID = taskAccessor.create(task("subtask", taskID));
	timeAccessor.create( Time_entry( taskID, 100, 200 ) );
	timeAccessor.create( Time_entry( subTaskID, 150, 200 ) );
	return *taskAccessor.by_id(taskID, start, stop);
}

TEST(TimeManagement, tasksTimeIs100)
{
	extended_task task1 = getTask();
	ASSERT_EQ(100, task1.time);
}

TEST(TimeManagement, tasksTotalTimeIs150)
{
	extended_task task1 = getTask();
	ASSERT_EQ(150, task1.total_time);
}

TEST(TimeManagement, staggerTest1RecordedTimePassesEnd)
{
	extended_task task1 = getTask(0, 150);
	ASSERT_EQ(50, task1.time);
}

TEST(TimeManagement, staggerTest2RecordedTimeStartsBeforeStart)
{
	extended_task task1 = getTask(150, 300);
	ASSERT_EQ(50, task1.time);
}

TEST(TimeManagement, staggerTest3RecordedTotalTimePassesEnd)
{
	extended_task task1 = getTask(0, 150);
	ASSERT_EQ(50, task1.total_time);
}

TEST(TimeManagement, staggerTest3RecordedTotalTimeStartsBeforeStart)
{
	extended_task task1 = getTask(150, 300);
	ASSERT_EQ(100, task1.total_time);
}

}