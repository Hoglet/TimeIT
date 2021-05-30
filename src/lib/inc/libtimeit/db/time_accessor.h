#ifndef TIMEIT_TIME_ACCESSOR_H
#define TIMEIT_TIME_ACCESSOR_H

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include "sqlite3.h"
#include "notifier.h"
#include "time_entry.h"

namespace libtimeit
{
class Notifier;
using namespace std;

class Time_accessor
{
	friend Database;
public:
	Time_accessor(Database& database);

	Time_id                 create(Time_entry item);
	optional<Time_entry>    by_id(Time_id id);
	[[nodiscard]] Time_list by_state(Time_entry_state state) const;
	bool                    update(Time_entry item);
	void                    remove(Time_id id);

	Task_id_list            latest_active_tasks(int amount);


	Time_list               time_list(Task_id task_ID, time_t start, time_t stop);
	Duration                duration_time(Task_id task_ID, time_t start, time_t stop);
	Time_list               times_changed_since(time_t timestamp= 0);
	Time_id                 uuid_to_id(UUID uuid);


	Duration                total_cumulative_time(Task_id taskID, time_t start, time_t stop);
	Task_id_list            currently_running();
	Task_id_list            active_tasks(time_t start, time_t stop) ;

protected:
	static void          create_table(Database& db);
	static void          drop_views(Database& database);
    static void          create_views(Database& database);
    void                 remove_short_time_spans();

private:
	Database&       database;

    Duration        time_passing_start_limit(Task_id id, time_t start, time_t stop);
    Duration        time_passing_end_limit(Task_id & id, time_t & start, time_t & stop);
    Duration        time_completely_within_limits(Task_id & id, time_t & start, time_t & stop);
    Task_id_list    children_id_list(Task_id id);

	Statement statement_uuid_to_id;
	Statement by_id_statement;
	Statement statement_update_time;
	Statement statement_new_entry;
	static void setup(Database& database);
	static void upgrade(Database& database);
	static void upgrade_to_db_5(Database& database);
	static void internal_create(const Time_entry &item, Statement& new_entry);
};
}
#endif