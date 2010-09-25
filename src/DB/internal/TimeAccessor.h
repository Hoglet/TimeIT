#ifndef TIMEACCESSOR_H_
#define TIMEACCESSOR_H_

#include <map>
#include <vector>
#include <list>
#include <ITimeAccessor.h>
#include <CSQL.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "TaskAccessor.h"

class TimeAccessor : public ITimeAccessor
{
public:
	TimeAccessor(const std::string& dbname, boost::shared_ptr<TaskAccessor>& taskAccessor);
	virtual ~TimeAccessor(){};
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime);
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime);
	virtual void changeEndTime(int64_t timeID, time_t stopTime);
	virtual void changeStartTime(int64_t timeID, time_t startTime);
	virtual void setRunning(int64_t timeID, bool running);
	//virtual  getLatestTasks(int amount); select distinct times.taskid from times  order by times.stop DESC limit 5
	virtual TimeEntry getByID(int64_t id);
	virtual void remove(int64_t id);
	virtual std::map<int64_t, TaskTime> getTimeList(time_t startTime, time_t stopTime);
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime);
private:
	DBAbstraction::CSQL db;
	boost::shared_ptr<TaskAccessor> m_taskAccesor;
};

#endif /*TIMEACCESSOR_H_*/
