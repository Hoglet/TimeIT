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
std::string getImagePath();
std::string get639LanguageString();
time_t now();
time_t getTime(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
time_t getBeginingOfDay(const time_t&);
time_t getEndOfDay(const time_t&);
time_t getBeginingOfWeek(const time_t&);
time_t getEndOfWeek(const time_t&);
time_t getBeginingOfMonth(const time_t&);
time_t getEndOfMonth(const time_t&);
time_t getBeginingOfYear(const time_t&);
time_t getEndOfYear(const time_t&);
int getDaysInMonth(const time_t &rawtime);
std::string seconds2hhmm(int64_t s);
std::string createDurationString(const time_t &from, const time_t &to, const time_t *next);
uint64_t millisecondsSinceEpoch();

template<typename ... Args>
std::string string_printf(const std::string &fmt, Args ... vs)
{
	size_t size = std::snprintf(nullptr, 0, fmt.c_str(), vs...) + 1;
	char temp_buffer[size];
	std::snprintf(temp_buffer, size, fmt.c_str(), vs...);
	return std::string(temp_buffer);
}
}
;

#endif /* UTILS_H_ */
