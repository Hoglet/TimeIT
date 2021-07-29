#include <gtest/gtest.h>
#include <libtimeit/db/extended_task.h>

using namespace libtimeit;

TEST(ExtendedTask, testTime)
{
	extended_task task1(task_id(), {}, "Test");
	ASSERT_EQ(0, task1.time);
}

TEST(ExtendedTask, testRunning)
{
	extended_task task1( task_id(), {}, "Test", 0, 0, false);
	extended_task task2( task_id(), {}, "Test2", 0, 0, true);
	ASSERT_EQ(false, task1.running);
	ASSERT_EQ(true, task2.running);
}
