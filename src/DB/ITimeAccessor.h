/*
 * ITimeAccessor.h
 *
 *  Created on: Mar 5, 2009
 *      Author: hoglet
 */

#ifndef ITIMEACCESSOR_H_
#define ITIMEACCESSOR_H_

#include <map>
#include <vector>
#include <string>
#include <stdint.h>

struct TaskTime
{
	std::string name;
	time_t duration;
};
struct TimeEntry
{
	int64_t id;
	int64_t taskID;
	time_t start;
	time_t stop;
	bool   running;
};

class ITimeAccessor
{
public:
	virtual ~ITimeAccessor() {};
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime) = 0;
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime) = 0;
	virtual void changeEndTime(int64_t timeID, time_t stopTime) = 0;
	virtual void changeStartTime(int64_t timeID, time_t startTime) = 0;
	virtual void setRunning(int64_t timeID, bool running) = 0;
	virtual TimeEntry getByID(int64_t id) = 0;
	virtual void remove(int64_t id) = 0;
	virtual std::map<int64_t, TaskTime> getTimeList(time_t startTime, time_t stopTime) = 0;
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime) = 0;
	virtual std::vector<int64_t> getLatestTasks(int amount) = 0;
	virtual int	getTime(int64_t taskID, time_t startTime, time_t stopTime) = 0;
};
#endif /* ITIMEACCESSOR_H_ */
