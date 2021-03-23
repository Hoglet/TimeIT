#ifndef TIMEIT_TIMEACCESSOR_H
#define TIMEIT_TIMEACCESSOR_H

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>

#include <libtimeit/db/Database.h>
#include "CSQL.h"
#include "Notifier.h"
#include "TimeEntry.h"

namespace libtimeit
{
class Notifier;
using namespace std;

typedef vector<TimeEntry> TimeList;
typedef int64_t                TaskID;
typedef int64_t                TimeID;
typedef vector<TaskID>    TaskIDList;



class TimeAccessor
{
public:
	TimeAccessor(Database& database);
	virtual ~TimeAccessor();

	virtual TimeID              newTime(TaskID taskID, time_t startTime, time_t stopTime);
	virtual void                updateTime(TimeID timeID, time_t startTime, time_t stopTime);
	virtual void                stopAllRunning();
	virtual void                setRunning(TimeID timeID, bool running);
	virtual TaskIDList          getLatestTasks(int amount);
	virtual optional<TimeEntry> getByID(TimeID id);
	virtual void                remove(TimeID id);
	virtual TimeList            getDetailTimeList(TaskID taskId,time_t startTime, time_t stopTime);
	virtual int	                getTime(TaskID taskID, time_t startTime, time_t stopTime);
	virtual TimeList            getTimesChangedSince(time_t timestamp=0);
	virtual TimeID              uuidToId(UUID uuid);
	virtual bool                update(const TimeEntry& item);
	virtual TimeID              newEntry(const TimeEntry& item);
	virtual time_t              getTotalTimeWithChildren(TaskID taskID, time_t start, time_t stop);
	virtual TaskIDList          getRunningTasks();
	virtual TaskIDList          getActiveTasks(time_t start, time_t stop) ;

	void createTable();
	void upgradeToDB5();
    void createViews();
    void removeShortTimeSpans();
private:
	Database& database;
    int getTimePassingStartLimit(int64_t taskID, time_t start,  time_t stop);
    int getTimePassingEndLimit(int64_t & taskID, time_t & start, time_t & stop);
    int getTimeCompletelyWithinLimits(int64_t & taskID, time_t & start, time_t & stop);
    vector<int64_t> getChildrenIDs(int64_t taskID);
    dbexception dbe;

};
}
#endif