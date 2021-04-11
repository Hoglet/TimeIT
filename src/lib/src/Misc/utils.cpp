/*
 * Utils.cpp
 *
 *  Created on: 20-Jun-08
 *      Author: hoglet
 */

#include "libtimeit/utils.h"
#include <sys/types.h>
#include <unistd.h>
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
	string retVal = setlocale(LC_ALL, nullptr);
	retVal = retVal.substr(0, 2);
	return retVal;
}

time_t beginning_of_day( time_t raw_time)
{
	struct tm *timeInfo;
	timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 0;
	timeInfo->tm_min = 0;
	timeInfo->tm_hour = 0;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}
time_t end_of_day( time_t raw_time)
{
	struct tm *timeInfo;
	timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 59;
	timeInfo->tm_min = 59;
	timeInfo->tm_hour = 23;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}

int getDayOfWeek( time_t raw_time)
{
	struct tm *timeInfo = localtime(&raw_time);
	int dow = timeInfo->tm_wday;
	int retVal = dow;

	int weekStartsOnDay = *(nl_langinfo(_NL_TIME_FIRST_WEEKDAY)) - 1;
	retVal = retVal - weekStartsOnDay;
	if (retVal < 0)
	{
		retVal = retVal + 7;
	}
	return retVal;
}

time_t beginning_of_week( time_t raw_time)
{
	struct tm *timeInfo;
	int dayOfWeek = getDayOfWeek(raw_time);
	time_t bow = raw_time - (dayOfWeek) * SECOND_PER_DAY;
	timeInfo = localtime(&bow);
	timeInfo->tm_sec = 0;
	timeInfo->tm_min = 0;
	timeInfo->tm_hour = 0;
	timeInfo->tm_wday = 1;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}

time_t end_of_week( time_t raw_time)
{
	struct tm *timeInfo;
	int dayOfWeek = getDayOfWeek(raw_time);
	time_t eow = raw_time + (6 - dayOfWeek) * SECOND_PER_DAY;
	timeInfo = localtime(&eow);
	timeInfo->tm_sec = 59;
	timeInfo->tm_min = 59;
	timeInfo->tm_hour = 23;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}
time_t beginning_of_month( time_t raw_time)
{
	struct tm *timeInfo;
	timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 0;
	timeInfo->tm_min = 0;
	timeInfo->tm_hour = 0;
	timeInfo->tm_mday = 1;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}
time_t end_of_month( time_t raw_time)
{
	struct tm *timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 59;
	timeInfo->tm_min = 59;
	timeInfo->tm_hour = 23;
	timeInfo->tm_mday = days_in_month(raw_time);
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}

time_t beginning_of_year( time_t raw_time)
{
	struct tm *timeInfo;
	timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 0;
	timeInfo->tm_min = 0;
	timeInfo->tm_hour = 0;
	timeInfo->tm_mday = 1;
	timeInfo->tm_mon = 0;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}
time_t end_of_year( time_t raw_time)
{
	struct tm *timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 59;
	timeInfo->tm_min = 59;
	timeInfo->tm_hour = 23;
	timeInfo->tm_mday = 31;
	timeInfo->tm_mon = 11;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}

int days_in_month( time_t raw_time)
{
	struct tm *timeInfo = localtime(&raw_time);
	timeInfo->tm_sec = 59;
	timeInfo->tm_min = 59;
	timeInfo->tm_hour = 23;
	switch (timeInfo->tm_mon)
	{
	case 1:
		if ((timeInfo->tm_year % 4 == 0 && timeInfo->tm_year % 100 != 0) || timeInfo->tm_year % 400 == 0)
		{
			timeInfo->tm_mday = 29;
		}
		else
		{
			timeInfo->tm_mday = 28;
		}
		break;
	case 3:
	case 5:
	case 8:
	case 10:
		timeInfo->tm_mday = 30;
		break;
	default:
		timeInfo->tm_mday = 31;
		break;
	}
	return timeInfo->tm_mday;
}

time_t to_time(int year, int month, int day, int hour, int min, int sec)
{
	time_t raw_time;
	time(&raw_time);
	struct tm *timeInfo;
	timeInfo = localtime(&raw_time);
	timeInfo->tm_year = year - 1900;
	timeInfo->tm_mon = month;
	timeInfo->tm_mday = day;
	timeInfo->tm_hour = hour;
	timeInfo->tm_min = min;
	timeInfo->tm_sec = sec;
	timeInfo->tm_isdst = -1;
	return mktime(timeInfo);
}

bool is_on_different_days( time_t one, time_t other)
{
	struct tm oneTime = *localtime(&one);
	struct tm otherTime = *localtime(&other);
	return oneTime.tm_year != otherTime.tm_year || oneTime.tm_mon != otherTime.tm_mon || oneTime.tm_mday != otherTime.tm_mday;
}

string day_of_week_abbreviation( time_t raw_time)
{
	struct tm *timeInfo = localtime(&raw_time);
	array<char,15> abbreviation{};
	int length = strftime(abbreviation.data(), abbreviation.size(), "%a", timeInfo);
	return length ? abbreviation.data() : "☼";
}

string seconds_2_hhmm(int64_t s)
{
	stringstream retVal;
	int64_t minutes;
	int64_t hours;
	hours = s / (60 * 60);
	s -= hours * (60 * 60);
	minutes = s / 60;
	retVal << (hours < 10 ? "\u2007" : "") << hours << " h " << (minutes < 10 ? hours ? "0" : "\u2007" : "") << minutes << " m";
	return retVal.str();
}

string small_numbers(string s)
{
	stringstream retVal;
	for (auto c: s)
	{
		switch (c) {
			case '0': retVal << "₀"; break;
			case '1': retVal << "₁"; break;
			case '2': retVal << "₂"; break;
			case '3': retVal << "₃"; break;
			case '4': retVal << "₄"; break;
			case '5': retVal << "₅"; break;
			case '6': retVal << "₆"; break;
			case '7': retVal << "₇"; break;
			case '8': retVal << "₈"; break;
			case '9': retVal << "₉"; break;
			case '-': retVal << "₋"; break;
			default: retVal << c;
		}
	}
	return retVal.str();
}
string date_string(time_t timestamp)
{
	struct tm day = *localtime(&timestamp);
	stringstream date_string;
	date_string << (day.tm_year + 1900)
		<< "-"
		<< setfill('0') << setw(2) << day.tm_mon + 1
		<< "-"
		<< setfill('0') << setw(2) << day.tm_mday;
	return date_string.str();
}

string time_span_string( time_t from, time_t to)
{
	stringstream retVal;
	struct tm fromTime = *localtime(&from);
	struct tm toTime = *localtime(&to);
	bool acrossDays = fromTime.tm_year != toTime.tm_year || fromTime.tm_mon != toTime.tm_mon || fromTime.tm_mday != toTime.tm_mday;
	retVal << (acrossDays ? " " : "\u2003")
			<< setfill('0') << setw(2) << fromTime.tm_hour << ":" << setfill('0') << setw(2) << fromTime.tm_min;
	retVal << " → ";
	if (acrossDays)
	{
		retVal << (toTime.tm_year + 1900) << "-" << setfill('0') << setw(2) << toTime.tm_mon + 1 << "-" << setfill('0') << setw(2) << toTime.tm_mday << " ";
	}
	retVal << setfill('0') << setw(2) << toTime.tm_hour << ":" << setfill('0') << setw(2) << toTime.tm_min;
	return retVal.str();
}

string duration_string( time_t from, time_t to)
{
	stringstream retVal;
	struct tm fromTime = *localtime(&from);
	struct tm toTime = *localtime(&to);
	bool acrossDays = fromTime.tm_year != toTime.tm_year || fromTime.tm_mon != toTime.tm_mon || fromTime.tm_mday != toTime.tm_mday;
	retVal << (acrossDays ? " " : "\u2003") << "= " << seconds_2_hhmm(difftime(to, from)) << (acrossDays ? "" : "\u2003");
	return retVal.str();
}

string idling_string( time_t to, time_t next)
{
	stringstream retVal;
	int64_t toNextMin = (int64_t)difftime(next, to) / 60;
	retVal << "⇣ " << small_numbers(to_string(toNextMin)) << " ₘ";
	return retVal.str();
}

time_t now()
{
	return time(nullptr);
}

uint64_t milliseconds_since_epoch()
{
	struct timeval tv;
	gettimeofday(&tv, 0);

	return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

}
