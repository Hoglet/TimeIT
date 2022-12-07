#include <gtest/gtest.h>
#include <libtimeit/datatypes/time_entry.h>

using namespace libtimeit;

time_entry get_test_time_entry()
{
	auto id = uuid::from_string( "00a600df-00da-414c-8078-0182005b0109");
	return {
			time_id(*id),
			task_id( uuid::from_string( "00a600df-00da-414c-8078-0182005b0107").value()),
			system_clock::from_time_t( 100 ),
			system_clock::from_time_t( 1100 ),
			stopped,
			system_clock::from_time_t( 1100 ),
			""};
}

TEST(TimeEntry, changeEndTime)
{
	time_entry te= get_test_time_entry( );
	auto now=system_clock::now();
	auto changed_item = te.with_stop( system_clock::from_time_t( 2000 ) );
	ASSERT_EQ(system_clock::from_time_t( 2000 ), changed_item.stop);
	ASSERT_TRUE(changed_item.changed >= now) << "Changed time should be at current time";
}

TEST(TimeEntry, setDeleted)
{
	time_entry te= get_test_time_entry( );
	auto now = system_clock::now();
	auto changed_item= te.with(deleted);
	ASSERT_TRUE(changed_item.state == deleted);
	ASSERT_TRUE(changed_item.changed >= now) << "Changed time should be at current time";
}
