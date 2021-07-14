#include <iostream>
#include <sstream>
#include <string>
#include <libtimeit/db/time_accessor.h>

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
		statement_uuid_to_id(db.prepare("SELECT id FROM times WHERE uuid=?;")),
		by_id_statement(db.prepare("SELECT taskID, start, stop, changed, uuid, task_UUID, state, comment FROM v_times WHERE id = ?")),
		statement_update_time(db.prepare(
R"(
	UPDATE
 		times
	SET
		uuid=?,
		taskID=?,
		start=?,
		stop=?,
		running=?,
		changed=?,
		deleted=?,
		state=?,
		comment=?
	WHERE
		id=?)")),
		statement_new_entry(db.prepare(NEW_TIME_ENTRY_STATEMENT))
{
}




void time_accessor::remove(int64_t id)
{
	auto te = by_id(id);
	if(te)
	{
		update(te->with(DELETED));
	}
}

optional<Time_entry> time_accessor::by_id(int64_t id)
{
	by_id_statement.bind_value(1, id);

	Query_result rows = by_id_statement.execute();
	for ( auto row: rows )
	{
		int column{0};
		auto    task_id    = static_cast<Task_id>(row[column++].integer());
		time_t  start     = row[column++].integer();
		time_t  stop      = row[column++].integer();
		time_t  changed   = row[column++].integer();
		auto    uuid      = UUID::from_string(row[column++].text());
		auto    task_uuid = UUID::from_string(row[column++].text());
		auto    state     = static_cast<Time_entry_state>(row[column++].integer());
		auto    comment   = row[column].text();

		if( uuid && task_uuid )
		{
			return Time_entry(id, *uuid, task_id, *task_uuid, start, stop, state, changed, comment);
		}
		break;
	}
	return {};
}


Duration time_accessor::duration_time(int64_t task_ID, time_t start, time_t stop)
{
	Duration time = time_completely_within_limits(task_ID, start, stop);
	if (stop > 0)
	{
		time += time_passing_end_limit(task_ID, start, stop);
		time += time_passing_start_limit(task_ID, start, stop);
	}
	return time;
}

Duration time_accessor::time_completely_within_limits(int64_t &taskID, time_t &start, time_t &stop)
{
	Duration time = 0;
	Query_result rows;
	if (stop > 0)
	{
		sql_statement statement_time_completely_within_limits = db.prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? AND state IS NOT 3;");
		statement_time_completely_within_limits.bind_value(1, taskID);
		statement_time_completely_within_limits.bind_value(2, start);
		statement_time_completely_within_limits.bind_value(3, stop);
		rows = statement_time_completely_within_limits.execute();
	}
	else
	{
		sql_statement statement_get_time = db.prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
		statement_get_time.bind_value(1, taskID);
		rows = statement_get_time.execute();
	}
	for (auto row: rows)
	{
		if (row[0].has_value())
		{
			time = (Duration)row[0].integer();
		}
		break;
	}
	return time;
}

Duration time_accessor::time_passing_end_limit(int64_t &taskID, time_t &start, time_t &stop)
{
	Duration time = 0;
	stringstream statement;
	statement << " SELECT SUM(" << stop << "-start) AS time  ";
	statement << " FROM times ";
	statement << " WHERE taskID = " << taskID;
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
			time = (Duration)row[0].integer();
		}
	}
	return time;
}

Duration time_accessor::time_passing_start_limit(int64_t taskID, time_t start, time_t stop)
{
	Duration time = 0;
	stringstream statement;
	statement << "SELECT SUM(stop-" << start << ") AS time ";
	statement << " FROM times";
	statement << " WHERE taskID = " << taskID;
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
			time = (Duration)row[0].integer();
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
				times.taskid
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
		Task_id id = row[0].integer();
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
		Task_id id = row[0].integer();
		result_list.push_back(id);
	}
	return result_list;
}

Time_list time_accessor::time_list(int64_t taskId, time_t startTime, time_t stopTime)
{
	Time_list result_list;
	stringstream statement;
	statement << "SELECT id, start, stop, state, changed, uuid, task_UUID, comment FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskId;
	statement << " AND deleted=0 ";
	statement << " ORDER BY start";

	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
	{
		int column{0};
		int64_t id       = row[column++].integer();
		time_t start     = row[column++].integer();
		time_t stop      = row[column++].integer();
		auto   state     = static_cast<Time_entry_state>(row[column++].integer());
		time_t changed   = row[column++].integer();
		auto   uuid      = UUID::from_string(row[column++].text());
		auto   task_uuid = UUID::from_string(row[column++].text());
		auto   comment   = row[column].text();

		if (uuid && task_uuid)
		{
			result_list.push_back(Time_entry(id, *uuid, taskId, *task_uuid, start, stop, state, changed, comment));
		}
	}
	return result_list;
}

Time_list time_accessor::times_changed_since(time_t timestamp)
{
	Time_list result;

	sql_statement statement = db.prepare("SELECT taskID, start, stop, state, changed, uuid, id, task_UUID, comment FROM v_times WHERE changed>=?");

	statement.bind_value(1, timestamp);

	Query_result rows = statement.execute();
	for (vector<data_cell> row : rows)
	{
		int column{0};
		Task_id task_id = row[column++].integer();
		time_t start    = row[column++].integer();
		time_t stop     = row[column++].integer();
		auto   state    = static_cast<Time_entry_state>(row[column++].integer());
		time_t changed  = row[column++].integer();
		auto uuid       = UUID::from_string(row[column++].text());
		int64_t id      = row[column++].integer();
		auto task_uuid  = UUID::from_string(row[column++].text());
		auto comment    = row[column].text();

		if(uuid && task_uuid)
		{
			Time_entry te(id, *uuid, task_id, *task_uuid, start, stop, state, changed, comment);
			result.push_back(te);
		}
	}
	return result;
}

Time_id time_accessor::uuid_to_id(UUID uuid)
{
	Time_id id = 0;
	statement_uuid_to_id.bind_value(1, uuid.c_str());
	Query_result rows = statement_uuid_to_id.execute();
	for (auto row : rows)
	{
		id = row[0].integer();
	}
	return id;
}

bool time_accessor::update(Time_entry item )
{
	int64_t id = item.id;
	auto existing_item = by_id(id);
	if (existing_item && item != *existing_item && item.changed >= existing_item->changed)
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
		statement_update_time.bind_value(index++, item.uuid.c_str());
		statement_update_time.bind_value(index++, item.task_id);
		statement_update_time.bind_value(index++, item.start);
		statement_update_time.bind_value(index++, item.stop);
		statement_update_time.bind_value(index++, (int64_t)running);
		statement_update_time.bind_value(index++, item.changed);
		statement_update_time.bind_value(index++, (int64_t)deleted);
		statement_update_time.bind_value(index++, item.state);
		statement_update_time.bind_value(index++, item.comment);
		statement_update_time.bind_value(index, item.id);

		statement_update_time.execute();

		db.send_notification(TASK_TIME_CHANGED, item.task_id);
		db.send_notification(TIME_ENTRY_CHANGED, item.id);

		return true;
	}
	return false;
}

task_id_list time_accessor::children_id_list(int64_t id)
{
	task_id_list result;
	sql_statement statement_get_children_i_ds = db.prepare("SELECT id FROM tasks WHERE parent=?;");
	statement_get_children_i_ds.bind_value(1, id);
	Query_result rows = statement_get_children_i_ds.execute();

	for (auto row : rows)
	{
		result.push_back(row[0].integer());
	}
	return result;
}

Duration time_accessor::total_cumulative_time(int64_t taskID, time_t start, time_t stop)
{
	Duration total_time = duration_time(taskID, start, stop);
	task_id_list children = children_id_list(taskID);
	for (auto child : children)
	{
		total_time += total_cumulative_time(child, start, stop);
	}
	return total_time;
}

Time_id time_accessor::create(Time_entry item)
{
	internal_create(item, statement_new_entry);
	Time_id time_id = db.id_of_last_insert();

	if (item.start != item.stop)
	{
		db.send_notification(TASK_UPDATED, item.task_id);
	}
	db.send_notification(TIME_ENTRY_CHANGED, time_id);
	return time_id;
}

void time_accessor::internal_create(const Time_entry &item, sql_statement& statement_new_entry)
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
	statement_new_entry.bind_value(index++, item.uuid.c_str());
	statement_new_entry.bind_value(index++, item.task_id);
	statement_new_entry.bind_value(index++, item.start);
	statement_new_entry.bind_value(index++, item.stop);
	statement_new_entry.bind_value(index++, item.changed);
	statement_new_entry.bind_value(index++, (int64_t)deleted);
	statement_new_entry.bind_value(index++, (int64_t)running);
	statement_new_entry.bind_value(index++, item.state);
	statement_new_entry.bind_value(index,   item.comment);
	statement_new_entry.execute();
}

void time_accessor::upgrade_to_db_5(database& db)
{
	time_t now = time(nullptr);

	db.execute("DELETE FROM times WHERE taskID = 0");
	db.execute("DROP TABLE IF EXISTS times_backup");
	db.execute("ALTER TABLE times RENAME TO times_backup");
	time_accessor::create_table(db);
	auto statement_new_entry(db.prepare(NEW_TIME_ENTRY_STATEMENT));

	sql_statement statement = db.prepare("SELECT id, taskID, start, stop FROM  times_backup");
	Query_result rows = statement.execute();
	for (vector<data_cell> row : rows)
	{
		int64_t id     = row[0].integer();
		int64_t task_id = row[1].integer();
		time_t  start  = row[2].integer();
		time_t  stop   = row[3].integer();

		Time_entry item(id, UUID(), task_id, {}, start, stop, STOPPED, now, "");
		internal_create(item, statement_new_entry);
	}
	db.execute("DROP TABLE IF EXISTS times_backup");
}
void time_accessor::upgrade(database& db)
{
	if (db.current_db_version() < 5 ) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		upgrade_to_db_5(db);
	}
	if(db.current_db_version() == 5) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (!db.column_exists("times", "state") )
		{
			db.execute(R"query(
				ALTER TABLE
					times
				ADD
					state INTEGER
				DEFAULT 0;
				)query");

			db.execute(R"query(
				UPDATE
					times
				SET
					state = 1
				WHERE
					running = 1;
				)query");

			db.execute(R"query(
				UPDATE
					times
				SET
					state = 3
				WHERE
					deleted = 1;
				)query");
		}
		if( ! db.column_exists("times", "comment") )
		{
			db.execute(
					R"query(
				ALTER TABLE
					times
				ADD
					comment VARCHAR;
				)query");
		}
	}
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
						times.taskid
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
		Task_id id = row[0].integer();
		result_list.push_back(id);
	}
	return result_list;
}

Time_list time_accessor::by_state(Time_entry_state state) const
{
	Time_list time_list;

	sql_statement statement = db.prepare(R"Query(
		SELECT
			id,
			taskID,
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
	statement.bind_value(1, state);

	Query_result rows = statement.execute();
	for (auto row: rows)
	{
		auto column{0};
		Time_id id         = row[column++].integer();
		Task_id task_id    = row[column++].integer();
		time_t  start      = row[column++].integer();
		time_t  stop       = row[column++].integer();
		auto    item_state = static_cast<Time_entry_state>(row[column++].integer());
		int64_t changed    = row[column++].integer();
		auto    uuid       = UUID::from_string(row[column++].text());
		auto    task_uuid  = UUID::from_string(row[column++].text());
		auto    comment    = row[column].text();

		if (uuid && task_uuid)
		{
			time_list.emplace_back( Time_entry(id, *uuid, task_id, *task_uuid, start, stop, item_state, changed, comment));
		}
	}
	return time_list;
}

void time_accessor::setup(database& db)
{
	time_accessor::create_table(db);
	time_accessor::drop_views(db);
	time_accessor::upgrade(db);
	time_accessor::create_views(db);
}

}

