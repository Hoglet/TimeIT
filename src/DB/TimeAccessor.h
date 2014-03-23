#ifndef TIMEACCESSOR_H_
#define TIMEACCESSOR_H_

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>

#include "CSQL.h"
#include "Notifier.h"
#include "TimeEntry.h"

namespace DB
{
class Notifier;

class ITimeAccessor
{
public:
	virtual ~ITimeAccessor();
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime) = 0;
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime) = 0;
	virtual void setRunning(int64_t timeID, bool running) = 0;
	virtual TimeEntry getByID(int64_t id) = 0;
	virtual void remove(int64_t id) = 0;
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime) = 0;
	virtual std::vector<int64_t> getLatestTasks(int amount) = 0;
	virtual int	getTime(int64_t taskID, time_t startTime, time_t stopTime) = 0;
	virtual std::shared_ptr<std::vector<TimeEntry> > getTimesChangedSince(time_t timestamp=0) = 0;
	virtual int64_t uuidToId(std::string uuid) = 0;
	virtual bool update(const TimeEntry& item) = 0;
	virtual int64_t newEntry(const TimeEntry& item) = 0;
	virtual time_t getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop) = 0;
	virtual std::vector<int64_t> getRunningTasks() = 0;

};

class TimeAccessor : public ITimeAccessor
{
public:
	TimeAccessor(std::shared_ptr<DBAbstraction::CSQL>& db, std::shared_ptr<Notifier>& notifier);
	virtual ~TimeAccessor();
	virtual int64_t newTime(int64_t taskID, time_t startTime, time_t stopTime);
	virtual void updateTime(int64_t timeID, time_t startTime, time_t stopTime);
	virtual void setRunning(int64_t timeID, bool running);
	virtual std::vector<int64_t> getLatestTasks(int amount);
	virtual TimeEntry getByID(int64_t id);
	virtual void remove(int64_t id);
	virtual std::vector<TimeEntry> getDetailTimeList(int64_t taskId,time_t startTime, time_t stopTime);
	virtual int	getTime(int64_t taskID, time_t startTime, time_t stopTime);
	virtual std::shared_ptr<std::vector<TimeEntry> > getTimesChangedSince(time_t timestamp=0);
	virtual int64_t uuidToId(std::string uuid);
	virtual bool update(const TimeEntry& item);
	virtual int64_t newEntry(const TimeEntry& item);
	virtual time_t getTotalTimeWithChildren(int64_t taskID, time_t start, time_t stop);
	virtual std::vector<int64_t> getRunningTasks();

	void createTable();
	void upgradeToDB5();
    void createViews();
    void createStatements();
private:
	std::shared_ptr<DBAbstraction::CSQL> db;
	std::shared_ptr<Notifier> notifier;
    int getTimePassingStartLimit(int64_t taskID, time_t start,  time_t stop);
    int getTimePassingEndLimit(int64_t & taskID, time_t & start, time_t & stop);
    int getTimeCompletelyWithinLimits(int64_t & taskID, time_t & start, time_t & stop);
    std::shared_ptr<std::vector<int64_t>> getChildrenIDs(int64_t taskID);

    std::shared_ptr<DBAbstraction::Statement> statement_timeCompletelyWithinLimits;
    std::shared_ptr<DBAbstraction::Statement> statement_getTime;
    std::shared_ptr<DBAbstraction::Statement> statement_newEntry;
    std::shared_ptr<DBAbstraction::Statement> statement_uuidToId;
	std::shared_ptr<DBAbstraction::Statement> statement_updateTime;

    dbexception dbe;
};
}
#endif /*TIMEACCESSOR_H_*/
