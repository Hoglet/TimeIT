#ifndef TIME_ACCESSOR_H
#define TIME_ACCESSOR_H

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <chrono>

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include "sqlite3.h"
#include "notifier.h"
#include "libtimeit/datatypes/time_entry.h"

namespace libtimeit
{
class notification_manager;
using namespace std;
using namespace std::chrono;

class time_accessor
{
	friend database;
public:
	explicit time_accessor(database& database);

	int64_t                 create(const time_entry& item);
	optional<time_entry>    by_id( time_id id);

	[[nodiscard]] time_list by_state( time_entry_state state) const;
	bool                    update(const time_entry& item);
	void                    remove(const time_entry& item);

	task_id_list            latest_active_tasks(int amount);


	time_list               by_activity( const task_id& owner, time_t start_time, time_t stop_time);

	seconds  duration_time(
			const task_id& id,
			time_point<system_clock> start = time_point<system_clock>::min(),
			time_point<system_clock> stop  = time_point<system_clock>::max()
					);
	seconds  total_cumulative_time(
			const task_id&           owner,
			time_point<system_clock> start = time_point<system_clock>::min(),
			time_point<system_clock> stop  = time_point<system_clock>::max()
					);

	Duration                duration_time( const task_id &task_id, time_t start, time_t stop );
	time_list               times_changed_since( time_t timestamp= 0);


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

	seconds         time_passing_start_limit(      const task_id& id, time_point<system_clock> start, time_point<system_clock> stop );
	seconds         time_passing_end_limit(        const task_id& id, time_point<system_clock> start, time_point<system_clock> stop );
	seconds         time_completely_within_limits( const task_id& id, time_point<system_clock> start, time_point<system_clock> stop );

    task_id_list    children_id_list(const task_id& id);

	sql_statement by_uuid_statement;
	sql_statement statement_update_time;
	sql_statement statement_new_entry;
	static void setup(database& db);
	static void internal_create( const time_entry &item, sql_statement& new_entry, database& db);
};
}
#endif