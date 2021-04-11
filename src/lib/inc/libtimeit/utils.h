/*
 * Utils.h
 *
 *  Created on: 20-Jun-08
 *      Author: hoglet
 */

#ifndef UTILS_H_
#define UTILS_H_
#include <iostream>
#include <string>

namespace libtimeit
{
using namespace std;

string image_path();
string ISO_639_language_string();
time_t now();
time_t to_time(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);

time_t beginning_of_day( time_t raw_time);
time_t end_of_day( time_t raw_time);

time_t beginning_of_week( time_t raw_time);
time_t end_of_week( time_t  raw_time);

time_t beginning_of_month( time_t raw_time);
time_t end_of_month( time_t raw_time);

time_t beginning_of_year( time_t raw_time);
time_t end_of_year( time_t raw_time);

int    days_in_month( time_t raw_time);
bool   is_on_different_days(time_t one, const time_t other);

string day_of_week_abbreviation( time_t raw_time);

string seconds_2_hhmm(int64_t s);
string date_string(time_t timestamp);
string time_span_string(time_t from, time_t to);
string duration_string(time_t from, time_t to);
string idling_string(time_t to, time_t next);

uint64_t milliseconds_since_epoch();

template<typename ... Args>
string string_printf(const string &fmt, Args ... vs)
{
	size_t size = snprintf(nullptr, 0, fmt.c_str(), vs...) + 1;
	char temp_buffer[size];
	snprintf(temp_buffer, size, fmt.c_str(), vs...);
	return string(temp_buffer);
}
}
;

#endif /* UTILS_H_ */
