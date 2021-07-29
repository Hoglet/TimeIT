#include <gtest/gtest.h>
#include <libtimeit/db/time_entry.h>

using namespace libtimeit;

Time_entry getTestTimeEntry()
{
	auto uuid = UUID::from_string("00a600df-00da-414c-8078-0182005b0109");
	return Time_entry(
			time_id(*uuid),
			task_id(UUID::from_string("00a600df-00da-414c-8078-0182005b0107").value()),
			100,
			1100,
			STOPPED,
			1100,
			"");
}

/*TEST(TimeEntry, changeStartTime)
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(nullptr);
	TimeEntry changedEntry=te.with_start(400);
	ASSERT_EQ(400, changedEntry.start());
	ASSERT_TRUE(changedEntry.last_changed()>=now) << "Changed time should be at current time";
}*/

TEST(TimeEntry, changeEndTime)
{
	Time_entry te=getTestTimeEntry();
	time_t now=time(0);
	auto changedItem = te.with_stop(2000);
	ASSERT_EQ(2000, changedItem.stop);
	ASSERT_TRUE(changedItem.changed >= now) << "Changed time should be at current time";
}

TEST(TimeEntry, setDeleted)
{
	Time_entry te=getTestTimeEntry();
	time_t now=time(0);
	auto changedItem= te.with(DELETED);
	ASSERT_TRUE(changedItem.state == DELETED);
	ASSERT_TRUE(changedItem.changed >= now) << "Changed time should be at current time";
}
