/*
 * Utils.cpp
 *
 *  Created on: 20-Jun-08
 *      Author: hoglet
 */

#include "libtimeit/utils.h"
#include <sys/types.h>
#include "libtimeit/OS_abstraction.h"
#include <ctime>
#include <iomanip>
#include <langinfo.h>
#include <sys/stat.h>
#include <sys/time.h>

//using namespace Glib;
using namespace std;

namespace libtimeit
{
constexpr auto SECOND_PER_DAY = 24 * 60 * 60;

//LCOV_EXCL_START
string image_path()
{
	return PROGRAMNAME_DATADIR;
}

string ISO_639_language_string()
{
	string return_value = setlocale(LC_ALL, nullptr);
	return_value = return_value.substr(0, 2);
	return return_value;
}

time_t beginning_of_day( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 0;
	time_info->tm_min = 0;
	time_info->tm_hour = 0;
	time_info->tm_isdst = -1;
	return mktime(time_info);
}
time_t end_of_day( time_t raw_time)
{
	struct tm * time_info = localtime(&raw_time);
	time_info->tm_sec = 59;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_min = 59;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_hour = 23; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_isdst = -1;
	return mktime(time_info);
}

int get_day_of_week(time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	int dow = time_info->tm_wday;
	int return_value = dow;

	int week_starts_on_day = *(nl_langinfo(_NL_TIME_FIRST_WEEKDAY)) - 1;
	return_value = return_value - week_starts_on_day;
	if (return_value < 0)
	{
		return_value = return_value + 7; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	}
	return return_value;
}

time_t beginning_of_week( time_t raw_time)
{
	int day_of_week = get_day_of_week(raw_time);
	time_t bow = raw_time - (day_of_week) * SECOND_PER_DAY;
	struct tm * time_info = localtime(&bow);
	time_info->tm_sec = 0;
	time_info->tm_min = 0;
	time_info->tm_hour = 0;
	time_info->tm_wday = 1;
	time_info->tm_isdst = -1;
	return mktime(time_info);
}

time_t end_of_week( time_t raw_time)
{

	int day_of_week = get_day_of_week(raw_time);
	time_t eow = raw_time + (6 - day_of_week) * SECOND_PER_DAY;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	struct tm * time_info = localtime(&eow);
	time_info->tm_sec = 59;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_min = 59;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_hour = 23;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_isdst = -1;
	return mktime(time_info);
}
time_t beginning_of_month( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 0;
	time_info->tm_min = 0;
	time_info->tm_hour = 0;
	time_info->tm_mday = 1;
	time_info->tm_isdst = -1;
	return mktime(time_info);
}
time_t end_of_month( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_min = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_hour = 23;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_mday = days_in_month(raw_time);
	time_info->tm_isdst = -1;
	return mktime(time_info);
}

time_t beginning_of_year( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 0;
	time_info->tm_min = 0;
	time_info->tm_hour = 0;
	time_info->tm_mday = 1;
	time_info->tm_mon = 0;
	time_info->tm_isdst = -1;
	return mktime(time_info);
}
time_t end_of_year( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_min = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_hour = 23;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_mday = 31;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_mon = 11;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_isdst = -1;
	return mktime(time_info);
}

int days_in_month( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	time_info->tm_sec = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_min = 59;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_hour = 23;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	switch (time_info->tm_mon)
	{
	case 1:
		if ((time_info->tm_year % 4 == 0 && time_info->tm_year % 100 != 0) || time_info->tm_year % 400 == 0) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		{
			time_info->tm_mday = 29; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		}
		else
		{
			time_info->tm_mday = 28; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		}
		break;
	case 3:
	case 5:    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	case 8:    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	case 10:   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		time_info->tm_mday = 30;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		break;
	default:
		time_info->tm_mday = 31;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		break;
	}
	return time_info->tm_mday;
}

time_t to_time(int year, int month, int day, int hour, int min, int sec)
{
	time_t raw_time = 0;
	time(&raw_time);
	struct tm*	time_info = localtime(&raw_time);
	time_info->tm_year = year - 1900;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	time_info->tm_mon = month;
	time_info->tm_mday = day;
	time_info->tm_hour = hour;
	time_info->tm_min = min;
	time_info->tm_sec = sec;
	time_info->tm_isdst = -1;
	return mktime(time_info);
}

bool is_on_different_days( time_t one, time_t other)
{
	struct tm one_time = *localtime(&one);
	struct tm other_time = *localtime(&other);
	return one_time.tm_year != other_time.tm_year || one_time.tm_mon != other_time.tm_mon || one_time.tm_mday != other_time.tm_mday;
}

string day_of_week_abbreviation( time_t raw_time)
{
	struct tm *time_info = localtime(&raw_time);
	array<char,15> abbreviation{};              // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	int length = (int)strftime(abbreviation.data(), abbreviation.size(), "%a", time_info);
	return length ? abbreviation.data() : "☼";
}

string seconds_2_hhmm(int64_t s)
{
	stringstream return_value;
	auto hours = s / (60 * 60);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	s -= hours * (60 * 60);      // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	auto minutes = s / 60;      // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	return_value << (hours < 10 ? "\u2007" : "")  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		<< hours << " h "
		<< (minutes < 10 ? hours ? "0" : "\u2007" : "") << minutes << " m"; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	return return_value.str();
}

string small_numbers(string s)
{
	stringstream return_value;
	for (auto c: s)
	{
		switch (c) {
			case '0': return_value << "₀"; break;
			case '1': return_value << "₁"; break;
			case '2': return_value << "₂"; break;
			case '3': return_value << "₃"; break;
			case '4': return_value << "₄"; break;
			case '5': return_value << "₅"; break;
			case '6': return_value << "₆"; break;
			case '7': return_value << "₇"; break;
			case '8': return_value << "₈"; break;
			case '9': return_value << "₉"; break;
			case '-': return_value << "₋"; break;
			default: return_value << c;
		}
	}
	return return_value.str();
}
string date_string(time_t time_stamp)
{
	struct tm day = *localtime(&time_stamp);
	stringstream date_string;
	date_string << (day.tm_year + 1900)   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		<< "-"
		<< setfill('0') << setw(2) << day.tm_mon + 1
		<< "-"
		<< setfill('0') << setw(2) << day.tm_mday;
	return date_string.str();
}

string time_span_string( time_t from, time_t to)
{
	stringstream return_value;
	struct tm from_time = *localtime(&from);
	struct tm to_time = *localtime(&to);
	bool across_days = from_time.tm_year != to_time.tm_year || from_time.tm_mon != to_time.tm_mon || from_time.tm_mday != to_time.tm_mday;
	return_value << (across_days ? " " : "\u2003")
				 << setfill('0') << setw(2) << from_time.tm_hour << ":" << setfill('0') << setw(2) << from_time.tm_min;
	return_value << " → ";
	if (across_days)
	{
		return_value << (to_time.tm_year + 1900) << "-" << setfill('0') << setw(2) << to_time.tm_mon + 1 << "-" << setfill('0') << setw(2) << to_time.tm_mday << " "; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	}
	return_value << setfill('0') << setw(2) << to_time.tm_hour << ":" << setfill('0') << setw(2) << to_time.tm_min;
	return return_value.str();
}

string duration_string( time_t from, time_t to)
{
	stringstream return_value;
	struct tm from_time = *localtime(&from);
	struct tm to_time = *localtime(&to);
	bool across_days = from_time.tm_year != to_time.tm_year || from_time.tm_mon != to_time.tm_mon || from_time.tm_mday != to_time.tm_mday;
	return_value << (across_days ? " " : "\u2003") << "= " << seconds_2_hhmm((int64_t)difftime(to, from)) << (across_days ? "" : "\u2003");
	return return_value.str();
}

string idling_string( time_t to, time_t next)
{
	stringstream return_value;
	auto to_next_min = (int64_t)difftime(next, to) / 60;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	return_value << "⇣ " << small_numbers(to_string(to_next_min)) << " ₘ";
	return return_value.str();
}

time_t now()
{
	return time(nullptr);
}

[[maybe_unused]] uint64_t milliseconds_since_epoch()
{
	struct timeval tv{};
	gettimeofday(&tv, nullptr);

	return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

}
