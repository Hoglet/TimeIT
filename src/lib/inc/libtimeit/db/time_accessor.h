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
class notification_manager;
using namespace std;

class time_accessor
{
	friend database;
public:
	time_accessor(database& database);

	int64_t                 create(const Time_entry& item);
	optional<Time_entry>    by_id(time_id id);

	[[nodiscard]] Time_list by_state(Time_entry_state state) const;
	bool                    update(const Time_entry& item);
	void                    remove(const Time_entry& item);

	task_id_list            latest_active_tasks(int amount);


	Time_list               time_list(const task_id& task_ID, time_t start, time_t stop);
	Duration                duration_time(const task_id& task_ID, time_t start, time_t stop);
	Time_list               times_changed_since(time_t timestamp= 0);

	Duration                total_cumulative_time(const task_id& owner, time_t start, time_t stop);
	task_id_list            currently_running();
	task_id_list            active_tasks(time_t start, time_t stop) ;

protected:
	static void          create_table(database& db);
	static void          drop_views(database& db);
    static void          create_views(database& db);
    void                 remove_short_time_spans();

private:
	database&       db;

    Duration        time_passing_start_limit(const task_id& id, time_t start, time_t stop);
    Duration        time_passing_end_limit(const task_id & id, time_t & start, time_t & stop);
    Duration        time_completely_within_limits(const task_id& id, time_t & start, time_t & stop);
    task_id_list    children_id_list(const task_id& id);

	sql_statement by_uuid_statement;
	sql_statement statement_update_time;
	sql_statement statement_new_entry;
	static void setup(database& db);
	static void internal_create(const Time_entry &item, sql_statement& new_entry, database& db);
};
}
#endif