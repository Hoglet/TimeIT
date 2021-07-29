#include <gtest/gtest.h>
#include <libtimeit/db/task.h>

using namespace libtimeit;



TEST (Task, with)
{
	task original("Apa", task_id(), false,  0, {}, false, 0, false);
	auto now = time(0);

	auto task1 = original.with_name("Test");
	ASSERT_EQ("Test", task1.name);

	task parent("Parent");
	auto task2 = original.with_parent(parent.id);
	ASSERT_EQ( parent.id, task2.parent_id);

	auto task3 = original.with_completed(true);
	ASSERT_EQ(true, task3.completed);

	auto task4 = original.with_deleted(true);
	ASSERT_EQ(true, task4.deleted);

	ASSERT_TRUE(abs(task4.last_changed - now) < 2);

}

