/*
 * MockTimeAccessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKTIMEACCESSOR_H_
#define MOCKTIMEACCESSOR_H_

#include <libtimeit/db/TimeAccessor.h>
namespace test
{
using namespace libtimeit;

class MockTimeAccessor: public ITimeAccessor
{
public:
	MockTimeAccessor();
	virtual ~MockTimeAccessor();
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime);
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime);
	virtual void changeEndTime(int64_t timeID, time_t stopTime);
	virtual void changeStartTime(int64_t timeID, time_t startTime);
	virtual void stopAllRunning();
	virtual void setRunning(int64_t timeID, bool running);
	virtual std::optional<TimeEntry> getByID(int64_t id);
	virtual void remove(int64_t id);
	virtual void removeShortTimeSpans();
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime) ;
	virtual std::vector<int64_t> getLatestTasks(int amount);
	virtual int	getTime(int64_t taskID, time_t startTime, time_t stopTime);
	std::vector<TimeEntry> getTimesChangedSince(time_t timestamp = 0);
	virtual int64_t uuidToId(UUID uuid);
	virtual bool update(const TimeEntry& item);
	virtual int64_t newEntry(const TimeEntry& item);
	virtual time_t getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop);
	virtual std::vector<int64_t> getRunningTasks();
	virtual std::vector<int64_t> getActiveTasks(time_t start, time_t stop);
};

} /* namespace Test */
#endif /* MOCKTIMEACCESSOR_H_ */
