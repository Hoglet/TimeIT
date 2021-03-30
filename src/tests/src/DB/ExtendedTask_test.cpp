#include <gtest/gtest.h>
#include <libtimeit/db/extended_task.h>

using namespace libtimeit;

TEST(ExtendedTask, testTime)
{
	Extended_task task(0, 0, "Test");
	ASSERT_EQ(0, task.time());
}

TEST(ExtendedTask, testRunning)
{
	Extended_task task(0, 0, "Test", 0, 0, false);
	Extended_task task2(0, 0, "Test2", 0, 0, true);
	ASSERT_EQ(false, task.running());
	ASSERT_EQ(true, task2.running());
}
