#include <gtest/gtest.h>
#include <DB/Task.h>

using namespace DB;



TEST( Task, getters)
{
	Task task("Name");
	ASSERT_EQ(0, task.ID());
	ASSERT_EQ(0, task.parentID());

	ASSERT_EQ("Name", task.name());
	ASSERT_EQ(false, task.completed());

}

TEST (Task, with)
{
	Task original("Apa", 0, UUID(), false, 0, 0, {}, false);
	auto now = time(0);

	auto task = original.withName("Test");
	ASSERT_EQ("Test", task.name());

	auto task2 = original.withParent(13);
	ASSERT_EQ(13, task2.parentID());

	auto task3 = original.withCompleted(true);
	ASSERT_EQ(true, task3.completed());

	auto task4 = original.withDeleted(true);
	ASSERT_EQ(true, task4.deleted());

	auto lastChanged = task4.lastChanged();
	ASSERT_TRUE(abs(lastChanged - now) < 2);

}

