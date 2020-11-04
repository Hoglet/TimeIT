#include "test_timeEntry.h"
#include "cute.h"
#include "cute_runner.h"
#include "TempDB.h"
#include <TimeEntry.h>

using namespace DB;

namespace Test
{
TimeEntry getTestTimeEntry()
{
	TimeEntry te(1, "00a600df-00da-414c-8078-0182005b0109", 1, "00a600df-00da-414c-8078-0182005b0107" , 100, 1100, false, false, 1100);
	return te;
}

void TimeEntry_changeStartTime()
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(0);
	te.setStart(400);
	ASSERT_EQUAL(400, te.getStart());
	ASSERTM("Changed time should be at current time", te.getLastChanged()>=now);
}

void TimeEntry_changeEndTime()
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(0);
	te.setStop(2000);
	ASSERT_EQUAL(2000, te.getStop());
	ASSERTM("Changed time should be at current time", te.getLastChanged()>=now);
}

void TimeEntry_setDeleted()
{
	TimeEntry te=getTestTimeEntry();
	time_t now=time(0);
	te.setDeleted(true);
	ASSERT_EQUAL(true, te.getDeleted());
	ASSERTM("Changed time should be at current time", te.getLastChanged()>=now);
}

cute::suite make_suite_test_TimeEntry()
{
	cute::suite s;
	s.push_back(CUTE(TimeEntry_changeStartTime));
	s.push_back(CUTE(TimeEntry_changeEndTime));
	s.push_back(CUTE(TimeEntry_setDeleted));
	return s;

}

} /* namespace Test */

