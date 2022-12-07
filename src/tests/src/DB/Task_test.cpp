#include <gtest/gtest.h>
#include <libtimeit/datatypes/task.h>

using namespace libtimeit;



TEST (Task, with)
{
	task original("Apa", task_id(), system_clock::from_time_t(0), {}, false, 0min, false);
	auto now = system_clock::now();

	auto task_1 = original.with_name( "Test");
	ASSERT_EQ( "Test", task_1.name);

	task parent("Parent");
	auto task_2 = original.with_parent( parent.id);
	ASSERT_EQ( parent.id, task_2.parent_id);

	auto task_4 = original.with_deleted( true);
	ASSERT_EQ( true, task_4.deleted);

	ASSERT_TRUE( task_4.last_changed - now >= 0s);
	ASSERT_TRUE( task_4.last_changed - now < 2s);

}
