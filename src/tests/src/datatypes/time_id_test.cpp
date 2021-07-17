#include <gtest/gtest.h>
#include <iostream>
#include <libtimeit/datatypes/time_id.h>

using namespace libtimeit;

TEST (UUID, compare_random_time_id )
{
	time_id one;
	time_id other;
	ASSERT_NE(one, other);
}



TEST (UUID, compare_equal_time_id )
{
	time_id one;
	time_id other(one);
	ASSERT_EQ(one, other);
}