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

namespace Utils
{
	std::string getImagePath();
	std::string get639LanguageString();
	time_t now();
	time_t getTime(int year,int month,int day,int hour=0, int min=0, int sec=0);
	time_t getBeginingOfDay(const time_t&);
	time_t getEndOfDay(const time_t&);
	time_t getBeginingOfWeek(const time_t&);
	time_t getEndOfWeek(const time_t&);
	time_t getBeginingOfMonth(const time_t&);
	time_t getEndOfMonth(const time_t&);
	time_t getBeginingOfYear(const time_t&);
	time_t getEndOfYear(const time_t&);
	int    getDaysInMonth(const time_t& rawtime);
	std::string seconds2ddhhmm(int64_t s);
	std::string seconds2hhmm(int64_t s);
	std::string createDurationString(const time_t& from,const time_t& to);
	uint64_t millisecondsSinceEpoch();
};

#endif /* UTILS_H_ */
