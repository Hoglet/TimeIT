#include <gtest/gtest.h>
#include <ctime>
#include <libtimeit/Utils.h>


TEST( Utils, testGetDaysInMonth )
{
	time_t rawtime = libtimeit::getTime(2013, 0, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in January";
	rawtime = libtimeit::getTime(2013, 1, 2, 2, 2, 2);
	ASSERT_EQ( 28, libtimeit::getDaysInMonth(rawtime) ) << "Days in February 2013";
	rawtime = libtimeit::getTime(2016, 1, 2, 2, 2, 2);
	ASSERT_EQ( 29, libtimeit::getDaysInMonth(rawtime) ) << "Days in February 2016";
	rawtime = libtimeit::getTime(2013, 2, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in March 2013";
	rawtime = libtimeit::getTime(2013, 3, 2, 2, 2, 2);
	ASSERT_EQ( 30, libtimeit::getDaysInMonth(rawtime) ) << "Days in April 2013";
	rawtime = libtimeit::getTime(2013, 4, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in May 2013";
	rawtime = libtimeit::getTime(2013, 5, 2, 2, 2, 2);
	ASSERT_EQ( 30, libtimeit::getDaysInMonth(rawtime) ) << "Days in June 2013";
	rawtime = libtimeit::getTime(2013, 6, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in July 2013";
	rawtime = libtimeit::getTime(2013, 7, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in August 2013";
	rawtime = libtimeit::getTime(2013, 8, 2, 2, 2, 2);
	ASSERT_EQ( 30, libtimeit::getDaysInMonth(rawtime) ) << "Days in September 2013";
	rawtime = libtimeit::getTime(2013, 9, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in October 2013";
	rawtime = libtimeit::getTime(2013, 10, 2, 2, 2, 2);
	ASSERT_EQ( 30, libtimeit::getDaysInMonth(rawtime) ) << "Days in November 2013";
	rawtime = libtimeit::getTime(2013, 4, 2, 2, 2, 2);
	ASSERT_EQ( 31, libtimeit::getDaysInMonth(rawtime) ) << "Days in December 2013";
}

TEST( Utils, convertSecondsToStrings )
{
	int timespan2 = 2 * 60 * 60 + 4 * 60;
	ASSERT_EQ(  std::string("\u20072 h 04 m"), libtimeit::seconds2hhmm(timespan2) ) <<
		"Converting 2 hours and 4 minutes into a text string";
}

TEST( Utils, convertTimespanToString )
{
	time_t start = libtimeit::getTime(2013, 0, 4, 11, 0, 0);
	time_t stop = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	std::string result = libtimeit::createDurationString(start, stop, nullptr);
	std::string expected = "2013-01-04\u200311:00 → 12:00\u2003= \u20071 h 00 m";
	ASSERT_EQ( expected, result ) << "Converting a time span of one hour into a text string";

	start = libtimeit::getTime(2012, 0, 4, 11, 0, 0);
	stop = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	result = libtimeit::createDurationString(start, stop, nullptr);
	expected = "2012-01-04 11:00 → 2013-01-04 12:00 = 8785 h 00 m";
	ASSERT_EQ( expected, result ) << "Converting a time span of one year into a text string";
}

TEST( Utils, dayEnd )
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getEndOfDay(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month";
	ASSERT_EQ(   4, end_of_day->tm_mday ) << "Check day";
	ASSERT_EQ(  23, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(  59, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(  59, end_of_day->tm_sec )  << "Check seconds";
}

TEST( Utils, beginingOfDay )
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getBeginingOfDay(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month";
	ASSERT_EQ(   4, end_of_day->tm_mday ) << "Check day";
	ASSERT_EQ(   0, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(   0, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(   0, end_of_day->tm_sec )  << "Check seconds";
}

TEST( Utils, beginingOfWeek )
{
	setlocale(LC_ALL, "C");
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getBeginingOfWeek(testPoint);
	struct tm *beginningOfWeek = localtime(&eod);
	ASSERT_EQ( 0, beginningOfWeek->tm_sec )    << "Check seconds";
	ASSERT_EQ( 0, beginningOfWeek->tm_min )    << "Check minute";
	ASSERT_EQ( 0, beginningOfWeek->tm_hour )   << "Check hour";
	ASSERT_EQ( 0, beginningOfWeek->tm_wday )   << "Check day of week";
	ASSERT_EQ( 30, beginningOfWeek->tm_mday)   << "Check day of month";
	ASSERT_EQ( 11, beginningOfWeek->tm_mon )   << "Check month";
	ASSERT_EQ( 112, beginningOfWeek->tm_year ) << "Check year";
}

TEST( Utils, endOfWeek )
{
	setlocale(LC_ALL, "C");
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getEndOfWeek(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 59, end_of_day->tm_sec )   << "Check seconds";
	ASSERT_EQ( 59, end_of_day->tm_min )   << "Check minute";
	ASSERT_EQ( 23, end_of_day->tm_hour )  << "Check hour";
	ASSERT_EQ( 5, end_of_day->tm_mday )   << "Check day of month";
	ASSERT_EQ( 59, end_of_day->tm_sec )   << "Check seconds";
	ASSERT_EQ( 0, end_of_day->tm_mon)     << "Check month";
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
}

void Utils_endOfWeek2()
{
	time_t testPoint = libtimeit::getTime(2013, 0, 6, 12, 0, 0);
	time_t eod = libtimeit::getEndOfWeek(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ(  59, end_of_day->tm_sec )  << "Check seconds";
	ASSERT_EQ(  59, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(  23, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(   6, end_of_day->tm_mday ) << "Check day of month";
	ASSERT_EQ(  59, end_of_day->tm_sec )  << "Check seconds";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month";
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year" ;
}
void Utils_beginningOfMonth()
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getBeginingOfMonth(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month" ;
	ASSERT_EQ(   1, end_of_day->tm_mday ) << "Check day";
	ASSERT_EQ(   0, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(   0, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(   0, end_of_day->tm_sec )  << "Check seconds";
}

TEST( Utils, endOfMonth )
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getEndOfMonth(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month";
	ASSERT_EQ(  31, end_of_day->tm_mday ) << "Check day";
	ASSERT_EQ(  23, end_of_day->tm_hour)  << "Check hour";
	ASSERT_EQ(  59, end_of_day->tm_min)   << "Check minute";
	ASSERT_EQ(  59, end_of_day->tm_sec)   << "Check seconds";
}


TEST( Utils, beginningOfYear )
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getBeginingOfYear(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day->tm_mon )  << "Check month";
	ASSERT_EQ(   1, end_of_day->tm_mday ) << "Check day";
	ASSERT_EQ(   0, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(   0, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(   0, end_of_day->tm_sec )  << "Check seconds";
}

TEST( Utils, endOfYear )
{
	time_t testPoint = libtimeit::getTime(2013, 0, 4, 12, 0, 0);
	time_t eod = libtimeit::getEndOfYear(testPoint);
	struct tm *end_of_day = localtime(&eod);
	ASSERT_EQ( 113, end_of_day->tm_year ) << "Check year";
	ASSERT_EQ(  11, end_of_day->tm_mon  ) << "Check month";
	ASSERT_EQ(  31, end_of_day->tm_mday ) <<"Check day";
	ASSERT_EQ(  23, end_of_day->tm_hour ) << "Check hour";
	ASSERT_EQ(  59, end_of_day->tm_min )  << "Check minute";
	ASSERT_EQ(  59, end_of_day->tm_sec )  << "Check seconds";
}


