/*
 * MockTimeAccessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockTimeAccessor.h"

namespace Test
{

MockTimeAccessor::MockTimeAccessor()
{
}

MockTimeAccessor::~MockTimeAccessor()
{
}

int64_t MockTimeAccessor::newTime(int64_t taskID, time_t startTime, time_t stopTime)
{
	return 1;
}

void MockTimeAccessor::updateTime(int64_t timeID, time_t startTime, time_t stopTime)
{
}

void MockTimeAccessor::changeEndTime(int64_t timeID, time_t stopTime)
{
}

void MockTimeAccessor::changeStartTime(int64_t timeID, time_t startTime)
{
}

void MockTimeAccessor::setRunning(int64_t timeID, bool running)
{
}

TimeEntry MockTimeAccessor::getByID(int64_t id)
{
	TimeEntry te;
	return te;
}

void MockTimeAccessor::remove(int64_t id)
{
}


std::vector<TimeEntry> MockTimeAccessor::getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime)
{
	std::vector<TimeEntry> list;
	return list;
}

std::vector<int64_t> MockTimeAccessor::getLatestTasks(int amount)
{
	std::vector<int64_t> list;
	return list;
}

int	MockTimeAccessor::getTime(int64_t taskID, time_t startTime, time_t stopTime)
{
	return 1;
}

} /* namespace Test */
