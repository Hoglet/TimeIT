#include <gtest/gtest.h>
#include <DB/ExtendedTask.h>

using namespace DB;

TEST(ExtendedTask, testTime)
{
	ExtendedTask task(0, 0, "Test");
	ASSERT_EQ(0, task.time());
}

TEST(ExtendedTask, testRunning)
{
	ExtendedTask task(0, 0, "Test", 0, 0, false);
	ExtendedTask task2(0, 0, "Test2", 0, 0, true);
	ASSERT_EQ(false, task.running());
	ASSERT_EQ(true, task2.running());
}
