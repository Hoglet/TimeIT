/*
 * MockTimeAccessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKTIMEACCESSOR_H_
#define MOCKTIMEACCESSOR_H_

#include "ITimeAccessor.h"
namespace Test
{

class MockTimeAccessor: public ITimeAccessor
{
public:
	MockTimeAccessor();
	virtual ~MockTimeAccessor();
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime);
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime);
	virtual void changeEndTime(int64_t timeID, time_t stopTime);
	virtual void changeStartTime(int64_t timeID, time_t startTime);
	virtual void setRunning(int64_t timeID, bool running);
	virtual TimeEntry getByID(int64_t id);
	virtual void remove(int64_t id);
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime) ;
	virtual std::vector<int64_t> getLatestTasks(int amount);
	virtual int	getTime(int64_t taskID, time_t startTime, time_t stopTime);
};

} /* namespace Test */
#endif /* MOCKTIMEACCESSOR_H_ */
