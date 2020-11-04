#include "test_utils.h"

#include "cute.h"
#include <time.h>
#include "Utils.h"

namespace Test
{

void Utils_testGetDaysInMonth()
{
	time_t rawtime = Utils::getTime(2013, 0, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in January", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 1, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in February 2013", 28, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2016, 1, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in February 2016", 29, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 2, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in March 2013", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 3, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in April 2013", 30, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 4, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in May 2013", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 5, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in June 2013", 30, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 6, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in July 2013", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 7, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in August 2013", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 8, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in September 2013", 30, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 9, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in October 2013", 31, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 10, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in November 2013", 30, Utils::getDaysInMonth(rawtime));
	rawtime = Utils::getTime(2013, 4, 2, 2, 2, 2);
	ASSERT_EQUALM("Days in December 2013", 31, Utils::getDaysInMonth(rawtime));
}

void Utils_convertSecondsToStrings()
{
	int timespan1 = 4 * 60 + 2 * 60 * 60 + 2 * 60 * 60 * 24;
	ASSERT_EQUALM("Converting 2 days 2 hours and 4 minutes into a text string", std::string("02:02:04"),
			Utils::seconds2ddhhmm(timespan1));
	int timespan2 = 2 * 60 * 60 + 4 * 60;
	ASSERT_EQUALM("Converting 2 hours and 4 minutes into a text string", std::string("02:04"),
			Utils::seconds2hhmm(timespan2));
}

void Utils_convertTimespanToString()
{
	time_t start = Utils::getTime(2013, 0, 4, 11, 0, 0);
	time_t stop = Utils::getTime(2013, 0, 4, 12, 0, 0);
	std::string result = Utils::createDurationString(start, stop);
	std::string expected = "2013-01-04 11:00 -> 12:00";
	ASSERT_EQUALM("Converting a time span of one hour into a text string", expected, result);

	start = Utils::getTime(2012, 0, 4, 11, 0, 0);
	stop = Utils::getTime(2013, 0, 4, 12, 0, 0);
	result = Utils::createDurationString(start, stop);
	expected = "2012-01-04 11:00 -> 2013-01-04 12:00";
	ASSERT_EQUALM("Converting a time span of one year into a text string", expected, result);
}

void Utils_dayEnd()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getEndOfDay(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 4, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 23, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 59, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
}

void Utils_beginingOfDay()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getBeginingOfDay(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 4, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 0, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 0, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 0, end_of_day->tm_sec);
}

void Utils_beginingOfWeek()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getBeginingOfWeek(testPoint);
	struct tm *beginningOfWeek = localtime(&eod);
	ASSERT_EQUALM("Check seconds", 0, beginningOfWeek->tm_sec);
	ASSERT_EQUALM("Check minute", 0, beginningOfWeek->tm_min);
	ASSERT_EQUALM("Check hour", 0, beginningOfWeek->tm_hour);
	ASSERT_EQUALM("Check day of week", 1, beginningOfWeek->tm_wday);
	ASSERT_EQUALM("Check day of month", 31, beginningOfWeek->tm_mday);
	ASSERT_EQUALM("Check month", 11, beginningOfWeek->tm_mon);
	ASSERT_EQUALM("Check year", 112, beginningOfWeek->tm_year);
}

void Utils_endOfWeek()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getEndOfWeek(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
	ASSERT_EQUALM("Check minute", 59, end_of_day->tm_min);
	ASSERT_EQUALM("Check hour", 23, end_of_day->tm_hour);
	ASSERT_EQUALM("Check day of month", 6, end_of_day->tm_mday);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
}

void Utils_endOfWeek2()
{
	time_t testPoint = Utils::getTime(2013, 0, 6, 12, 0, 0);
	time_t eod = Utils::getEndOfWeek(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
	ASSERT_EQUALM("Check minute", 59, end_of_day->tm_min);
	ASSERT_EQUALM("Check hour", 23, end_of_day->tm_hour);
	ASSERT_EQUALM("Check day of month", 6, end_of_day->tm_mday);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
}
void Utils_beginningOfMonth()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getBeginingOfMonth(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 1, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 0, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 0, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 0, end_of_day->tm_sec);
}

void Utils_endOfMonth()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getEndOfMonth(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 31, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 23, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 59, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
}


void Utils_beginningOfYear()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getBeginingOfYear(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 0, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 1, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 0, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 0, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 0, end_of_day->tm_sec);
}

void Utils_endOfYear()
{
	time_t testPoint = Utils::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = Utils::getEndOfYear(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQUALM("Check year", 113, end_of_day->tm_year);
	ASSERT_EQUALM("Check month", 11, end_of_day->tm_mon);
	ASSERT_EQUALM("Check day", 31, end_of_day->tm_mday);
	ASSERT_EQUALM("Check hour", 23, end_of_day->tm_hour);
	ASSERT_EQUALM("Check minute", 59, end_of_day->tm_min);
	ASSERT_EQUALM("Check seconds", 59, end_of_day->tm_sec);
}



cute::suite make_suite_UtilsTest()
{
	cute::suite s;
	s.push_back(CUTE(Utils_testGetDaysInMonth));
	s.push_back(CUTE(Utils_convertSecondsToStrings));
	s.push_back(CUTE(Utils_convertTimespanToString));
	s.push_back(CUTE(Utils_dayEnd));
	s.push_back(CUTE(Utils_beginingOfDay));
	s.push_back(CUTE(Utils_beginingOfWeek));
	s.push_back(CUTE(Utils_endOfWeek));
	s.push_back(CUTE(Utils_beginningOfMonth));
	s.push_back(CUTE(Utils_beginningOfYear));
	s.push_back(CUTE(Utils_endOfMonth));
	s.push_back(CUTE(Utils_endOfYear));
	s.push_back(CUTE(Utils_endOfWeek2));
	return s;
}

} /* namespace Test */
