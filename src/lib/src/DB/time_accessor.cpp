#include <iostream>
#include <sstream>
#include <string>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/utils.h>
#include "legacy_db_helper.h"

using namespace std;
namespace libtimeit
{

static const string NEW_TIME_ENTRY_STATEMENT = R"Query(
	INSERT INTO
		times (uuid,taskID, start, stop, changed,deleted,running,state, comment)
	VALUES (?,?,?,?,?,?,?,?,?))Query";

time_accessor::time_accessor(database &op_database)
	:
		db(op_database),
		by_uuid_statement(db.prepare("SELECT start, stop, changed, uuid, task_UUID, state, comment FROM v_times WHERE uuid = ?")),
		statement_update_time(db.prepare(
R"(
	UPDATE
 		times
	SET
		start=?,
		stop=?,
		running=?,
		changed=?,
		deleted=?,
		state=?,
		comment=?
	WHERE
		uuid=?)")),
		statement_new_entry(db.prepare(NEW_TIME_ENTRY_STATEMENT))
{
}



void time_accessor::remove(const Time_entry& item)
{
	update(item.with(DELETED));
}

optional<Time_entry> time_accessor::by_id(time_id uuid)
{
	by_uuid_statement.bind_value(1, static_cast<string>(uuid));

	Query_result rows = by_uuid_statement.execute();
	for ( auto row: rows )
	{
		int column{0};
		time_t  start     = row[column++].integer();
		time_t  stop      = row[column++].integer();
		time_t  changed   = row[column++].integer();
		auto    possible_uuid      = UUID::from_string( row[column++].text());
		auto    task_uuid = optional_task_id(row[column++].text());
		auto    state     = static_cast<Time_entry_state>(row[column++].integer());
		auto    comment   = row[column].text();

		if( possible_uuid.has_value() && task_uuid.has_value() )
		{
			return Time_entry(
					time_id(possible_uuid.value()),
					task_uuid.value(),
					start,
					stop,
					state,
					changed,
					comment);
		}
		break;
	}
	return {};
}

Duration time_accessor::duration_time(const task_id& id, time_t raw_start, time_t raw_stop)
{
	auto start  = system_clock::from_time_t(raw_start);
	auto stop   = system_clock::from_time_t(raw_stop);

	auto result = duration_time(id, start, stop);
	return result.count();
}

seconds  time_accessor::duration_time(const task_id& id, time_point<system_clock> start, time_point<system_clock> stop)
{
	auto time = time_completely_within_limits(id, start, stop);
	if (stop < time_point<system_clock>::max())
	{
		time += time_passing_end_limit(id, start, stop);
		time += time_passing_start_limit(id, start, stop);
	}
	return time;
}



seconds time_accessor::time_completely_within_limits(const task_id& owner_id, time_point<system_clock> start, time_point<system_clock> stop)
{
	seconds time(0);
	Query_result rows;
	auto task_id = legacy_db_helper::new_task_id_to_old( owner_id, db);
	if (stop < time_point<system_clock>::max() )
	{
		sql_statement statement_time_completely_within_limits = db.prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? AND state IS NOT 3;");
		statement_time_completely_within_limits.bind_value( 1, task_id);
		statement_time_completely_within_limits.bind_value(2, system_clock::to_time_t(start) );
		statement_time_completely_within_limits.bind_value(3, system_clock::to_time_t(stop) );
		rows = statement_time_completely_within_limits.execute();
	}
	else
	{
		sql_statement statement_get_time = db.prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
		statement_get_time.bind_value( 1, task_id);
		rows = statement_get_time.execute();
	}
	for (auto row: rows)
	{
		if (row[0].has_value())
		{
			time = seconds(row[0].integer());
		}
		break;
	}
	return time;
}

seconds time_accessor::time_passing_end_limit( const task_id& owner, time_point<system_clock> start_point, time_point<system_clock> stop_point)
{
	auto start = duration_cast<seconds>( start_point.time_since_epoch() ).count();
	auto stop  = duration_cast<seconds>(  stop_point.time_since_epoch() ).count();

	int64_t owner_id = legacy_db_helper::new_task_id_to_old( owner, db);
	seconds time( 0 );
	stringstream statement;
	statement << " SELECT SUM(" << stop << "-start) AS time  ";
	statement << " FROM times ";
	statement << " WHERE taskID = " << owner_id;
	statement << " AND start < " << stop;
	statement << " AND stop  > " << stop;
	statement << " AND start > " << start;
	statement << " AND state IS NOT 3 ";
	Query_result rows = db.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = seconds(  row[0].integer() );
		}
	}
	return time;
}

seconds time_accessor::time_passing_start_limit(
		const task_id& owner_id,
		time_point<system_clock> start_point,
		time_point<system_clock> stop_point )
{
	auto start = duration_cast<seconds>( start_point.time_since_epoch() ).count();
	auto stop  = duration_cast<seconds>(  stop_point.time_since_epoch() ).count();

	auto task_id = legacy_db_helper::new_task_id_to_old( owner_id, db);

	seconds time( 0 );
	stringstream statement;
	statement << "SELECT SUM(stop-" << start << ") AS time ";
	statement << " FROM times";
	statement << " WHERE taskID = " << task_id;
	statement << " AND start <  " << start;
	statement << " AND stop  >  " << start;
	statement << " AND stop  <  " << stop;
	statement << " AND state IS NOT 3 ";
	Query_result rows = db.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = seconds(row[0].integer());
		}
	}
	return time;
}

task_id_list time_accessor::latest_active_tasks(int amount)
{
	task_id_list result_list;
	stringstream statement;
	statement << R"Query(
			SELECT DISTINCT
				tasks.uuid
			FROM
				times
			JOIN
				tasks
			ON
				times.taskID = tasks.id
				AND
				tasks.deleted='0'
			 ORDER BY
					times.stop
			DESC
			LIMIT
			)Query"
			<< amount;

	Query_result rows = db.execute(statement.str());

	for (auto row : rows)
	{
		auto id = task_id(UUID::from_string(row[0].text()).value());
		result_list.push_back(id);
	}
	return result_list;

}

task_id_list time_accessor::currently_running()
{
	task_id_list result_list;
	sql_statement statement_get_running_tasks = db.prepare(
			"SELECT DISTINCT times.taskid FROM times WHERE times.state IS 1;");

	Query_result rows = statement_get_running_tasks.execute();

	for (auto row : rows)
	{
		Task_id old_id = row[0].integer();
		//result_list.push_back(id);

		/** temp fix **/
		sql_statement statement_get_uuid = db.prepare(
				"SELECT uuid FROM tasks WHERE id=?;");
		statement_get_uuid.bind_value(1, old_id);
		Query_result rows2 = statement_get_uuid.execute();
		for (auto row2 : rows2)
		{
			auto id = task_id(UUID::from_string(row2[0].text()).value());
			result_list.push_back(id);
		}
		/* end temp fix */
	}
	return result_list;
}

Time_list time_accessor::time_list( const task_id& owner, time_t startTime, time_t stopTime)
{
	int64_t id = legacy_db_helper::new_task_id_to_old( owner, db);
	Time_list result_list;
	stringstream statement;
	statement << "SELECT start, stop, state, changed, uuid, task_UUID, comment FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << id;
	statement << " AND deleted=0 ";
	statement << " ORDER BY start";

	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
	{
		int column{0};
		time_t start     = row[column++].integer();
		time_t stop      = row[column++].integer();
		auto   state     = static_cast<Time_entry_state>(row[column++].integer());
		time_t changed   = row[column++].integer();
		auto   uuid      = UUID::from_string(row[column++].text());
		auto   task_uuid = UUID::from_string(row[column++].text());
		auto   comment   = row[column].text();

		if (uuid.has_value() && task_uuid.has_value())
		{
			result_list.push_back(
					Time_entry(
							time_id(uuid.value()),
							task_id( task_uuid.value()),
							start,
							stop,
							state,
							changed,
							comment));
		}
	}
	return result_list;
}

Time_list time_accessor::times_changed_since(time_t timestamp)
{
	Time_list result;

	sql_statement statement = db.prepare("SELECT start, stop, state, changed, uuid, task_UUID, comment FROM v_times WHERE changed>=?");

	statement.bind_value(1, timestamp);

	Query_result rows = statement.execute();
	for (vector<data_cell> row : rows)
	{
		int column{0};
		time_t start     = row[column++].integer();
		time_t stop      = row[column++].integer();
		auto   state     = static_cast<Time_entry_state>(row[column++].integer());
		time_t changed   = row[column++].integer();
		auto   uuid      = UUID::from_string(row[column++].text());
		auto   task_uuid = UUID::from_string(row[column++].text());
		auto   comment   = row[column].text();

		if(uuid && task_uuid)
		{
			auto id = time_id(*uuid);
			auto owner_id = task_id(task_uuid.value());
			result.emplace_back(
					id,
					owner_id,
					start,
					stop,
					state,
					changed,
					comment
					);
		}
	}
	return result;
}

bool time_accessor::update(const Time_entry& item )
{
	auto existing_item = by_id(item.id);
	if (
			existing_item.has_value() &&
			item != existing_item.value() &&
			item.changed >= existing_item->changed
			)
	{
		auto deleted = false;
		auto running = false;
		switch ( item.state )
		{
			case RUNNING:
				running = true;
				break;
			case DELETED:
				deleted = true;
				break;
			case STOPPED:
			case PAUSED:
				break;
		}
		auto index{1};
		statement_update_time.bind_value(index++, item.start);
		statement_update_time.bind_value(index++, item.stop);
		statement_update_time.bind_value(index++, static_cast<int64_t>(running) );
		statement_update_time.bind_value(index++, item.changed);
		statement_update_time.bind_value(index++, static_cast<int64_t>(deleted) );
		statement_update_time.bind_value(index++, static_cast<int64_t>(item.state));
		statement_update_time.bind_value(index++, item.comment);
		statement_update_time.bind_value(index,   static_cast<string>(item.id) );

		statement_update_time.execute();

		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_task_time_changed( item.owner_id );
				});

		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_time_entry_changed( item );
				});

		return true;
	}
	return false;
}

task_id_list time_accessor::children_id_list(const task_id& owner_id)
{
	auto id = legacy_db_helper::new_task_id_to_old(owner_id, db);
	task_id_list result;
	sql_statement statement_get_children_i_ds = db.prepare("SELECT uuid FROM tasks WHERE parent=?;");
	statement_get_children_i_ds.bind_value(1, id);
	Query_result rows = statement_get_children_i_ds.execute();

	for (auto row : rows)
	{
		result.push_back(optional_task_id(row[0].text()).value());
	}
	return result;
}

seconds  time_accessor::total_cumulative_time(
		const task_id&           id,
		time_point<system_clock> start,
		time_point<system_clock> stop
				)
{
	auto total_time = duration_time( id, start, stop);
	task_id_list children = children_id_list( id );
	for (auto child : children)
	{
		total_time += total_cumulative_time(child, start, stop);
	}
	return total_time;

}

Duration time_accessor::total_cumulative_time(const task_id& id, time_t raw_start, time_t raw_stop)
{
	auto start = system_clock::from_time_t( raw_start );
	auto stop  = system_clock::from_time_t( raw_stop );
	return total_cumulative_time( id, start, stop ).count();
}

int64_t time_accessor::create(const Time_entry& item)
{
	internal_create(item, statement_new_entry, db);
	int64_t id = db.id_of_last_insert();

	if (item.start != item.stop)
	{
		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_task_updated( item.owner_id );
				});
	}
	db.broadcast(
			[item](event_observer* observer)
			{
				observer->on_time_entry_changed( item );
			});
	return id;
}

void time_accessor::internal_create(const Time_entry &item, sql_statement& statement_new_entry, database& db)
{
	auto deleted = false;
	auto running = false;
	switch ( item.state )
	{
		case RUNNING:
			running = true;
			break;
		case DELETED:
			deleted = true;
			break;
		case STOPPED:
		case PAUSED:
			break;
	}
	auto index{1};
	auto owner_old_id = legacy_db_helper::new_task_id_to_old( item.owner_id, db);
	statement_new_entry.bind_value(index++, static_cast<string>(item.id));
	statement_new_entry.bind_value(index++, owner_old_id);
	statement_new_entry.bind_value(index++, item.start);
	statement_new_entry.bind_value(index++, item.stop);
	statement_new_entry.bind_value(index++, item.changed);
	statement_new_entry.bind_value(index++, deleted);
	statement_new_entry.bind_value(index++, running);
	statement_new_entry.bind_value(index++, static_cast<int64_t>(item.state));
	statement_new_entry.bind_value(index,   item.comment);
	statement_new_entry.execute();
}

void time_accessor::create_table(database& db)
{
	db.execute(R"Query(
			CREATE TABLE IF NOT EXISTS times
			(id          INTEGER PRIMARY KEY,
			 uuid        TEXT UNIQUE,
			 taskID      INTEGER,
			 start       INTEGER,
			 stop        INTEGER,
			 changed     INTEGER,
			 deleted     BOOL    DEFAULT 0,
			 running     BOOL    DEFAULT 0,
			 state       INTEGER DEFAULT 0,
			 comment     VARCHAR,
			 FOREIGN KEY(taskID) REFERENCES tasks(id)
			)
		)Query"
		);
}

void time_accessor::drop_views(database& db)
{
	db.execute("DROP VIEW IF EXISTS v_times");
}
void time_accessor::create_views(database& db)
{
	time_accessor::drop_views(db);
	db.execute(R"Query(
			CREATE VIEW v_times AS
			SELECT
				times.*,
				tasks.uuid AS task_UUID
			FROM
				times
			LEFT JOIN
				tasks
			ON times.taskID = tasks.id
		)Query"
	);
}

void time_accessor::remove_short_time_spans()
{
	db.execute("DELETE FROM times WHERE stop-start < 60");
}


task_id_list time_accessor::active_tasks(time_t start, time_t stop)
{
	task_id_list result_list;
	sql_statement statement_get_tasks = db.prepare(R"Query(
					SELECT DISTINCT
						tasks.uuid
					FROM
						times
					JOIN
						tasks
						ON
							times.taskID = tasks.id
								AND
							tasks.deleted='0'
					WHERE
						(times.start>=? AND times.start<=?)
						OR
						(times.stop>=? AND times.stop<=?)
						OR
						(times.start<? AND times.stop>?)
						AND
						times.state IS NOT 3;
					)Query"
					);
	auto index{1};
	statement_get_tasks.bind_value(index++, start);
	statement_get_tasks.bind_value(index++, stop);
	statement_get_tasks.bind_value(index++, start);
	statement_get_tasks.bind_value(index++, stop);
	statement_get_tasks.bind_value(index++, start);
	statement_get_tasks.bind_value(index, stop);

	Query_result rows = statement_get_tasks.execute();

	for (auto row : rows)
	{
		auto id = task_id(UUID::from_string(row[0].text()).value());
		result_list.push_back(id);
	}
	return result_list;
}

Time_list time_accessor::by_state(Time_entry_state state) const
{
	Time_list time_list;

	sql_statement statement = db.prepare(R"Query(
		SELECT
			start,
			stop,
			state,
			changed,
			uuid,
			task_UUID,
			comment
		FROM
			v_times
		WHERE
			state = ?
		)Query");
	statement.bind_value(1, static_cast<int>(state));

	Query_result rows = statement.execute();
	for (auto row: rows)
	{
		auto column{0};
		time_t  start      = row[column++].integer();
		time_t  stop       = row[column++].integer();
		auto    item_state = static_cast<Time_entry_state>(row[column++].integer());
		int64_t changed    = row[column++].integer();
		auto    uuid       = UUID::from_string(row[column++].text());
		auto    task_uuid  = UUID::from_string(row[column++].text());
		auto    comment    = row[column].text();

		if (uuid && task_uuid)
		{
			auto owner_id = task_id(task_uuid.value());
			time_list.emplace_back(
					time_id(*uuid),
					owner_id,
					start,
					stop,
					item_state,
					changed,
					comment);
		}
	}
	return time_list;
}

void time_accessor::setup(database& db)
{
	time_accessor::create_table(db);
	time_accessor::drop_views(db);
	time_accessor::create_views(db);
}

}

