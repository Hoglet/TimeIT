#include <gtest/gtest.h>
#include <ctime>
#include <libtimeit/utils.h>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace libtimeit;

TEST( utils, test_get_days_in_month )
{
	auto rawtime = to_time(2013, 0, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in January";
	rawtime = to_time(2013, 1, 2, 2, 2, 2);
	ASSERT_EQ( 28, days_in_month(rawtime) ) << "Days in February 2013";
	rawtime = to_time(2016, 1, 2, 2, 2, 2);
	ASSERT_EQ( 29, days_in_month(rawtime) ) << "Days in February 2016";
	rawtime = to_time(2013, 2, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in March 2013";
	rawtime = to_time(2013, 3, 2, 2, 2, 2);
	ASSERT_EQ( 30, days_in_month(rawtime) ) << "Days in April 2013";
	rawtime = to_time(2013, 4, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in May 2013";
	rawtime = to_time(2013, 5, 2, 2, 2, 2);
	ASSERT_EQ( 30, days_in_month(rawtime) ) << "Days in June 2013";
	rawtime = to_time(2013, 6, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in July 2013";
	rawtime = to_time(2013, 7, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in August 2013";
	rawtime = to_time(2013, 8, 2, 2, 2, 2);
	ASSERT_EQ( 30, days_in_month(rawtime) ) << "Days in September 2013";
	rawtime = to_time(2013, 9, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in October 2013";
	rawtime = to_time(2013, 10, 2, 2, 2, 2);
	ASSERT_EQ( 30, days_in_month(rawtime) ) << "Days in November 2013";
	rawtime = to_time(2013, 4, 2, 2, 2, 2);
	ASSERT_EQ( 31, days_in_month(rawtime) ) << "Days in December 2013";
}

TEST( utils, convert_seconds_to_strings )
{
	auto timespan2 = 2h  + 4min;
	auto expected = std::string("\u20072 h \u20074 m");
	auto actual   = seconds_2_hhmm(timespan2);
	ASSERT_EQ( expected , actual ) << "Converting 2 hours and 4 minutes into a txt string";
}

TEST( utils, convertTimespanToString )
{
	auto start = to_time(2013, 0, 4, 11, 0, 0);
	auto stop = to_time(2013, 0, 4, 12, 0, 0);
	std::string result = time_span_string(start, stop);
	std::string expected = "\u200311:00 → 12:00";
	ASSERT_EQ( expected, result ) << "Converting a time span of one hour into a txt string";

	start = to_time(2012, 0, 4, 11, 0, 0);
	stop = to_time(2013, 0, 4, 12, 0, 0);
	result = time_span_string(start, stop);
	expected = "\u200311:00 → 2013-01-04 12:00";
	ASSERT_EQ( expected, result ) << "Converting a time span of one year into a txt string";
}


TEST( utils, dayEnd )
{
	auto testPoint = to_time(2013, 0, 4, 12, 0, 0);
	auto eod = end_of_day(testPoint);
	struct tm end_of_day = localtime( eod );
	ASSERT_EQ( 113, end_of_day.tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day.tm_mon )  << "Check month";
	ASSERT_EQ(   4, end_of_day.tm_mday ) << "Check day";
	ASSERT_EQ(  23, end_of_day.tm_hour ) << "Check hour";
	ASSERT_EQ(  59, end_of_day.tm_min )  << "Check minute";
	ASSERT_EQ(  59, end_of_day.tm_sec )  << "Check seconds";
}

TEST( utils, beginingOfDay )
{
	auto testPoint = to_time(2013, 0, 4, 12, 0, 0);
	auto eod = beginning_of_day(testPoint);
	struct tm end_of_day = localtime( eod );
	ASSERT_EQ( 113, end_of_day.tm_year ) << "Check year";
	ASSERT_EQ(   0, end_of_day.tm_mon )  << "Check month";
	ASSERT_EQ(   4, end_of_day.tm_mday ) << "Check day";
	ASSERT_EQ(   0, end_of_day.tm_hour ) << "Check hour";
	ASSERT_EQ(   0, end_of_day.tm_min )  << "Check minute";
	ASSERT_EQ(   0, end_of_day.tm_sec )  << "Check seconds";
}

TEST( Utils, beginingOfWeek )
{
	setlocale(LC_ALL, "en_GB");
	auto test_point = to_time( 2013, 0, 4, 12, 0, 0);
	auto bow = beginning_of_week( test_point);
	struct tm beginning_of_week = localtime( bow);
	ASSERT_EQ( 0, beginning_of_week.tm_sec ) << "Check seconds";
	ASSERT_EQ( 0, beginning_of_week.tm_min ) << "Check minute";
	ASSERT_EQ( 0, beginning_of_week.tm_hour ) << "Check hour";
	ASSERT_EQ( 0, beginning_of_week.tm_wday ) << "Check day of week";
	ASSERT_EQ( 30, beginning_of_week.tm_mday) << "Check day of month";
	ASSERT_EQ( 11, beginning_of_week.tm_mon ) << "Check month";
	ASSERT_EQ( 112, beginning_of_week.tm_year ) << "Check year";
}

TEST( Utils, endOfWeek )
{
	setlocale(LC_ALL, "C");
	auto test_point = to_time( 2013, 0, 4, 12, 0, 0);
	auto eow = end_of_week( test_point);
	struct tm end_of_week = localtime( eow);
	ASSERT_EQ( 59, end_of_week.tm_sec ) << "Check seconds";
	ASSERT_EQ( 59, end_of_week.tm_min ) << "Check minute";
	ASSERT_EQ( 23, end_of_week.tm_hour ) << "Check hour";
	ASSERT_EQ( 5, end_of_week.tm_mday ) << "Check day of month";
	ASSERT_EQ( 59, end_of_week.tm_sec ) << "Check seconds";
	ASSERT_EQ( 0, end_of_week.tm_mon) << "Check month";
	ASSERT_EQ( 113, end_of_week.tm_year ) << "Check year";
}

TEST( Utils, endOfWeek2)
{
	setlocale(LC_ALL, "C");
	auto test_point = to_time(2013, 0, 5, 12, 0, 0);
	auto eow = end_of_week( test_point);
	struct tm end_of_week = localtime( eow);
	ASSERT_EQ( 59, end_of_week.tm_sec ) << "Check seconds";
	ASSERT_EQ( 59, end_of_week.tm_min ) << "Check minute";
	ASSERT_EQ( 23, end_of_week.tm_hour ) << "Check hour";
	ASSERT_EQ( 5, end_of_week.tm_mday ) << "Check day of month";
	ASSERT_EQ( 59, end_of_week.tm_sec ) << "Check seconds";
	ASSERT_EQ( 0, end_of_week.tm_mon ) << "Check month";
	ASSERT_EQ( 113, end_of_week.tm_year ) << "Check year" ;
}

TEST(  Utils, beginningOfMonth )
{
	setlocale(LC_ALL, "C");
	auto test_point = to_time(2013, 0, 4, 12, 0, 0);
	struct tm result = localtime( beginning_of_month( test_point) );
	ASSERT_EQ( 113, result.tm_year ) << "Check year";
	ASSERT_EQ( 0,   result.tm_mon ) << "Check month" ;
	ASSERT_EQ( 1,   result.tm_mday ) << "Check day";
	ASSERT_EQ( 0,   result.tm_hour ) << "Check hour";
	ASSERT_EQ( 0,   result.tm_min ) << "Check minute";
	ASSERT_EQ( 0,   result.tm_sec ) << "Check seconds";
}

TEST( Utils, endOfMonth )
{
	auto test_point = to_time(2013, 0, 4, 12, 0, 0);
	auto eom = end_of_month( test_point);
	struct tm end_of_month = localtime( eom );
	ASSERT_EQ( 113, end_of_month.tm_year ) << "Check year";
	ASSERT_EQ( 0, end_of_month.tm_mon ) << "Check month";
	ASSERT_EQ( 31, end_of_month.tm_mday ) << "Check day";
	ASSERT_EQ( 23, end_of_month.tm_hour) << "Check hour";
	ASSERT_EQ( 59, end_of_month.tm_min) << "Check minute";
	ASSERT_EQ( 59, end_of_month.tm_sec) << "Check seconds";
}


TEST( Utils, beginningOfYear )
{
	auto test_point = to_time( 2013, 0, 4, 12, 0, 0);
	auto eod =  beginning_of_year( test_point) ;
	struct tm beginning_of_year = localtime( eod);
	ASSERT_EQ( 113, beginning_of_year.tm_year ) << "Check year";
	ASSERT_EQ( 0, beginning_of_year.tm_mon ) << "Check month";
	ASSERT_EQ( 1, beginning_of_year.tm_mday ) << "Check day";
	ASSERT_EQ( 0, beginning_of_year.tm_hour ) << "Check hour";
	ASSERT_EQ( 0, beginning_of_year.tm_min ) << "Check minute";
	ASSERT_EQ( 0, beginning_of_year.tm_sec ) << "Check seconds";
}

TEST( Utils, endOfYear )
{
	auto test_point = to_time( 2013, 0, 4, 12, 0, 0);
	auto eoy = end_of_year( test_point);
	struct tm result = localtime( eoy);
	ASSERT_EQ( 113, result.tm_year ) << "Check year";
	ASSERT_EQ( 11, result.tm_mon  ) << "Check month";
	ASSERT_EQ( 31, result.tm_mday ) << "Check day";
	ASSERT_EQ( 23, result.tm_hour ) << "Check hour";
	ASSERT_EQ( 59, result.tm_min ) << "Check minute";
	ASSERT_EQ( 59, result.tm_sec ) << "Check seconds";
}

TEST( Utils, safe_strcpy_normal)
{
	const int BUFFER_SIZE=5;
	char buffer[BUFFER_SIZE]{"a"};
	const char * source="ab";
	auto result=safe_strcpy(buffer, source, BUFFER_SIZE);

	ASSERT_EQ(result, 0);
	ASSERT_STREQ(buffer, source);
}

TEST( Utils, safe_strcpy_to_long_source)
{
	const int BUFFER_SIZE=5;

	char buffer[BUFFER_SIZE]{"a"};
	const char * source="abcde";
	auto result=safe_strcpy(buffer, source, BUFFER_SIZE);

	ASSERT_EQ(result, 2);
	ASSERT_STREQ(buffer, "a");
}

TEST( Utils, safe_strcpy_null_source)
{
	const int BUFFER_SIZE=5;
	char buffer[BUFFER_SIZE]{"a"};
	auto result=safe_strcpy(buffer, nullptr, BUFFER_SIZE);

	ASSERT_EQ(result, 1);
	ASSERT_STREQ(buffer, "a");
}
TEST( Utils, safe_strcpy_null_buffer)
{
	const int BUFFER_SIZE=5;
	const char * source="abcde";
	auto result=safe_strcpy(nullptr, source, BUFFER_SIZE);

	ASSERT_EQ(result, 1);
}

TEST( Utils, trim_left)
{
	const auto original = R"(
  S O M E T I N G
   )";
	const auto expected = R"(S O M E T I N G
   )";
	const auto result = trim_left(original);
	ASSERT_EQ(result,expected);
}


TEST( Utils, trim_right)
{
	const auto original = R"(
  S O M E T I N G
   )";
	const auto expected = R"(
  S O M E T I N G)";
	const auto result = trim_right(original);
	ASSERT_EQ(result,expected);
}

TEST( Utils, trim)
{
	const auto original = R"(
   S O M E T I N G
   )";
	const auto expected = R"(S O M E T I N G)";
	const auto result = trim(original);
	ASSERT_EQ(result,expected);
}



TEST( Utils, abbreviate_string_empty)
{
	string original;
	string expected = "";
	auto result = abbreviate_string(original, 1000);
	ASSERT_EQ(result, expected);
}

TEST( Utils, abbreviate_string_short_line)
{
	string original = R"(This is the first line)";
	string expected = "This is the first line";
	auto result = abbreviate_string(original, 1000);
	ASSERT_EQ(result, expected);
}

TEST( Utils, abbreviate_string_long_line)
{
	string original = R"(This is the first line)";
	string expected = "This i ...";
	auto result = abbreviate_string(original, 10);
	ASSERT_EQ(result, expected);
}

TEST( Utils, abbreviate_string_multi_line)
{
	string original = R"(This is the first line
This is the second line)";
	string expected = "This is the first line ...";
	auto result = abbreviate_string(original, 100);
	ASSERT_EQ(result, expected);
}

TEST( Utils, abbreviate_string_empty_lines)
{
	string original = R"(

This is the third line)";
	string expected = "This is the third line";
	auto result = abbreviate_string(original, 100);
	ASSERT_EQ(result, expected);
}

