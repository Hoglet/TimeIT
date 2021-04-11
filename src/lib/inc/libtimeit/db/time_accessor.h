#ifndef TIMEIT_TIME_ACCESSOR_H
#define TIMEIT_TIME_ACCESSOR_H

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/accessor.h>
#include "sqlite3.h"
#include "notifier.h"
#include "time_entry.h"

namespace libtimeit
{
class Notifier;
using namespace std;

class Time_accessor: public Accessor
{
	friend Database;
public:
	Time_accessor(Database& database);
	~Time_accessor() = default;

	Time_ID              create(Time_entry item);
	optional<Time_entry> by_ID(Time_ID id);
	bool                 update(Time_entry item);
	void                 remove(Time_ID id);

	void                 stop_all();
	void                 setRunning(Time_ID time_ID, bool running);
	Task_ID_list         latest_active_tasks(int amount);


	Time_list            time_list(Task_ID task_ID, time_t start, time_t stop);
	Duration             duration_time(Task_ID task_ID, time_t start, time_t stop);
	Time_list            times_changed_since(time_t timestamp= 0);
	Time_ID              uuid_to_id(UUID uuid);


	Duration             total_cumulative_time(Task_ID taskID, time_t start, time_t stop);
	Task_ID_list         currently_running();
	Task_ID_list         active_tasks(time_t start, time_t stop) ;

protected:
	void                 create_table()   override;
    void                 create_views()   override;
	void                 upgrade()        override;
	void                 upgrade_to_DB5();

    void                 remove_short_time_spans();

private:
	Database&       database;

    Duration        time_passing_start_limit(int64_t taskID, time_t start, time_t stop);
    Duration        time_passing_end_limit(int64_t & taskID, time_t & start, time_t & stop);
    Duration        time_completely_within_limits(int64_t & taskID, time_t & start, time_t & stop);
    Task_ID_list    children_IDs(int64_t taskID);

};
}
#endif