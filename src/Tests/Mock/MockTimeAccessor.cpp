/*
 * MockTimeAccessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockTimeAccessor.h"

using namespace DB;

namespace Test
{

MockTimeAccessor::MockTimeAccessor()
{
}

MockTimeAccessor::~MockTimeAccessor()
{
}

void MockTimeAccessor::stopAllRunning()
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
	TimeEntry te(0, "", 0, "", 0, 0, false, false, 0);
	return te;
}

void MockTimeAccessor::remove(int64_t id)
{
}

std::vector<TimeEntry> MockTimeAccessor::getDetailTimeList(int64_t taskId, time_t startTime, time_t stopTime)
{
	std::vector<TimeEntry> list;
	return list;
}

std::vector<int64_t> MockTimeAccessor::getLatestTasks(int amount)
{
	std::vector<int64_t> list;
	return list;
}

int MockTimeAccessor::getTime(int64_t taskID, time_t startTime, time_t stopTime)
{
	return 1;
}

std::shared_ptr<std::vector<TimeEntry> > MockTimeAccessor::getTimesChangedSince(time_t timestamp)
{
	std::shared_ptr<std::vector<TimeEntry> > result;
	return result;
}
int64_t MockTimeAccessor::uuidToId(std::string uuid)
{
	return 0;
}

bool MockTimeAccessor::update(const TimeEntry& item)
{
	return false;
}

int64_t MockTimeAccessor::newEntry(const TimeEntry& item)
{
	return 0;
}
time_t MockTimeAccessor::getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop)
{
	return 0;
}
std::vector<int64_t> MockTimeAccessor::getRunningTasks()
{
	std::vector<int64_t> result;
	return result;
}

} /* namespace Test */
