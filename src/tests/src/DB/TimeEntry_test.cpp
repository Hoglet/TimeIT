#include "gtest/gtest.h"
#include "TimeEntry.h"

using namespace DB;

TimeEntry getTestTimeEntry()
{
	TimeEntry te(1, "00a600df-00da-414c-8078-0182005b0109", 1, "00a600df-00da-414c-8078-0182005b0107" , 100, 1100, false, false, 1100);
	return te;
}

/*TEST(TimeEntry, changeStartTime)
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(nullptr);
	TimeEntry changedEntry=te.withStart(400);
	ASSERT_EQ(400, changedEntry.start());
	ASSERT_TRUE(changedEntry.lastChanged()>=now) << "Changed time should be at current time";
}*/

TEST(TimeEntry, changeEndTime)
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(0);
	auto changedItem = te.withStop(2000);
	ASSERT_EQ(2000, changedItem.stop());
	ASSERT_TRUE(changedItem.changed()>=now) << "Changed time should be at current time";
}

TEST(TimeEntry, setDeleted)
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(0);
	auto changedItem=te.withDeleted(true);
	ASSERT_TRUE(changedItem.deleted());
	ASSERT_TRUE(changedItem.changed()>=now) << "Changed time should be at current time";
}



