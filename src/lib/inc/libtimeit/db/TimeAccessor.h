#ifndef TIMEIT_TIMEACCESSOR_H
#define TIMEIT_TIMEACCESSOR_H

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>

#include "CSQL.h"
#include "Notifier.h"
#include "TimeEntry.h"

namespace libtimeit
{
class Notifier;

using std::optional;

typedef std::vector<TimeEntry> TimeList;
typedef int64_t                TaskID;
typedef int64_t                TimeID;
typedef std::vector<TaskID>    TaskIDList;

class ITimeAccessor
{
public:
	virtual ~ITimeAccessor();
	virtual TimeID              newTime(TaskID taskID, time_t startTime, time_t stopTime) = 0;
	virtual void                updateTime(TimeID timeID, time_t startTime, time_t stopTime) = 0;
	virtual void                setRunning(TimeID timeID, bool running) = 0;
	virtual void                stopAllRunning() = 0;
	virtual optional<TimeEntry> getByID(TimeID id) = 0;
	virtual void                remove(TimeID id) = 0;
	virtual void                removeShortTimeSpans() = 0;
	virtual TimeList            getDetailTimeList(TaskID taskId,time_t startTime, time_t stopTime) = 0;
	virtual TaskIDList          getLatestTasks(int amount) = 0;
	virtual int	                getTime(int64_t taskID, time_t startTime, time_t stopTime) = 0;
	virtual TimeList            getTimesChangedSince(time_t timestamp=0) = 0;
	virtual TimeID              uuidToId(UUID uuid) = 0;
	virtual bool                update(const TimeEntry& item) = 0;
	virtual TimeID              newEntry(const TimeEntry& item) = 0;
	virtual time_t              getTotalTimeWithChildren(TaskID taskID, time_t start, time_t stop) = 0;
	virtual TaskIDList          getRunningTasks() = 0;
	virtual TaskIDList          getActiveTasks(time_t start, time_t stop) = 0;
};

class TimeAccessor : public ITimeAccessor
{
public:
	TimeAccessor(std::shared_ptr<CSQL>& db, Notifier& notifier);
	virtual ~TimeAccessor();

	virtual TimeID              newTime(TaskID taskID, time_t startTime, time_t stopTime);
	virtual void                updateTime(TimeID timeID, time_t startTime, time_t stopTime);
	virtual void                stopAllRunning();
	virtual void                setRunning(TimeID timeID, bool running);
	virtual TaskIDList          getLatestTasks(int amount);
	virtual optional<TimeEntry> getByID(TimeID id);
	virtual void                remove(TimeID id);
	virtual void                removeShortTimeSpans();
	virtual TimeList            getDetailTimeList(TaskID taskId,time_t startTime, time_t stopTime);
	virtual int	                getTime(TaskID taskID, time_t startTime, time_t stopTime);
	virtual TimeList            getTimesChangedSince(time_t timestamp=0);
	virtual TimeID              uuidToId(UUID uuid);
	virtual bool                update(const TimeEntry& item);
	virtual TimeID              newEntry(const TimeEntry& item);
	virtual time_t              getTotalTimeWithChildren(TaskID taskID, time_t start, time_t stop);
	virtual TaskIDList          getRunningTasks();
	virtual TaskIDList          getActiveTasks(time_t start, time_t stop);

	void createTable();
	void upgradeToDB5();
	void createViews();
private:
	std::shared_ptr<CSQL> db;
	Notifier& notifier;
    int getTimePassingStartLimit(int64_t taskID, time_t start,  time_t stop);
    int getTimePassingEndLimit(int64_t & taskID, time_t & start, time_t & stop);
    int getTimeCompletelyWithinLimits(int64_t & taskID, time_t & start, time_t & stop);
    std::shared_ptr<std::vector<int64_t>> getChildrenIDs(int64_t taskID);
    dbexception dbe;

};
}
#endif