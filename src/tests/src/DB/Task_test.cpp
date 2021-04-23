#include <gtest/gtest.h>
#include <libtimeit/db/task.h>

using namespace libtimeit;



TEST( Task, getters)
{
	Task task("Name");
	ASSERT_EQ(0, task.id);
	ASSERT_EQ(0, task.parent_id);

	ASSERT_EQ("Name", task.name);
	ASSERT_EQ(false, task.completed);

}

TEST (Task, with)
{
	Task original("Apa", 0, UUID(), false, 0, 0, {}, false, 0, false);
	auto now = time(0);

	auto task = original.with_name("Test");
	ASSERT_EQ("Test", task.name);

	auto task2 = original.with_parent(13);
	ASSERT_EQ(13, task2.parent_id);

	auto task3 = original.with_completed(true);
	ASSERT_EQ(true, task3.completed);

	auto task4 = original.with_deleted(true);
	ASSERT_EQ(true, task4.deleted);

	ASSERT_TRUE(abs(task4.last_changed - now) < 2);

}

