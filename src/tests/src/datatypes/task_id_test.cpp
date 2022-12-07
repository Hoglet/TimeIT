#include <gtest/gtest.h>
#include <libtimeit/datatypes/task_id.h>

using namespace libtimeit;

TEST (UUID, compare_random_task_id )
{
	task_id one;
	task_id other;
	ASSERT_NE(one, other);
}



TEST (UUID, compare_equal_task_id )
{
	task_id one;
	task_id other(one);
	ASSERT_EQ(one, other);
}
