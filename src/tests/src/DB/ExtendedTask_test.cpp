#include <gtest/gtest.h>
#include <libtimeit/db/extended_task.h>

using namespace libtimeit;

TEST(ExtendedTask, testTime)
{
	task temp_task("Hej");
	extended_task task_1( temp_task, 4, 0, false, false);
	ASSERT_EQ(4, task_1.time);
}

TEST(ExtendedTask, testRunning)
{
	task temp_task_1("task1");
	task temp_task_2("task2");
	extended_task task_1( temp_task_1, 0, 0, false, false);
	extended_task task_2( temp_task_2, 0, 0, false, true);
	ASSERT_EQ(false, task_1.running);
	ASSERT_EQ(true, task_2.running);
}
