#include <iostream>
#include <sstream>
#include <string>
#include <libtimeit/db/time_accessor.h>

using namespace std;
namespace libtimeit
{

static const string new_time_entry_statement = R"Query(
	INSERT INTO
		times (uuid,taskID, start, stop, changed,deleted,running,state, comment)
	VALUES (?,?,?,?,?,?,?,?,?))Query";

Time_accessor::Time_accessor(Database &op_database)
	:
	database(op_database),
	statement_uuid_to_id(database.prepare("SELECT id FROM times WHERE uuid=?;")),
	by_id_statement(database.prepare("SELECT taskID, start, stop, changed, uuid, task_UUID, state, comment FROM v_times WHERE id = ?")),
	statement_update_time(database.prepare(
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
	statement_new_entry( database.prepare(new_time_entry_statement))
{
}




void Time_accessor::remove(int64_t id)
{
	auto te = by_id(id);
	if(te)
	{
		update(te->with(DELETED));
	}
}

optional<Time_entry> Time_accessor::by_id(int64_t id)
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


Duration Time_accessor::duration_time(int64_t task_ID, time_t start, time_t stop)
{
	Duration time = time_completely_within_limits(task_ID, start, stop);
	if (stop > 0)
	{
		time += time_passing_end_limit(task_ID, start, stop);
		time += time_passing_start_limit(task_ID, start, stop);
	}
	return time;
}

Duration Time_accessor::time_completely_within_limits(int64_t &taskID, time_t &start, time_t &stop)
{
	Duration time = 0;
	Query_result rows;
	if (stop > 0)
	{
		Statement statement_time_completely_within_limits = database.prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? AND state IS NOT 3;");
		statement_time_completely_within_limits.bind_value(1, taskID);
		statement_time_completely_within_limits.bind_value(2, start);
		statement_time_completely_within_limits.bind_value(3, stop);
		rows = statement_time_completely_within_limits.execute();
	}
	else
	{
		Statement statement_get_time = database.prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
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

Duration Time_accessor::time_passing_end_limit(int64_t &taskID, time_t &start, time_t &stop)
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
	Query_result rows = database.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = (Duration)row[0].integer();
		}
	}
	return time;
}

Duration Time_accessor::time_passing_start_limit(int64_t taskID, time_t start, time_t stop)
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
	Query_result rows = database.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = (Duration)row[0].integer();
		}
	}
	return time;
}

Task_id_list Time_accessor::latest_active_tasks(int amount)
{
	Task_id_list result_list;
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

	Query_result rows = database.execute(statement.str());

	for (auto row : rows)
	{
		Task_id id = row[0].integer();
		result_list.push_back(id);
	}
	return result_list;

}

Task_id_list Time_accessor::currently_running()
{
	Task_id_list result_list;
	Statement statement_get_running_tasks = database.prepare(
			"SELECT DISTINCT times.taskid FROM times WHERE times.state IS 1;");

	Query_result rows = statement_get_running_tasks.execute();

	for (auto row : rows)
	{
		Task_id id = row[0].integer();
		result_list.push_back(id);
	}
	return result_list;
}

Time_list Time_accessor::time_list(int64_t taskId, time_t startTime, time_t stopTime)
{
	Time_list result_list;
	stringstream statement;
	statement << "SELECT id, start, stop, state, changed, uuid, task_UUID, comment FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskId;
	statement << " AND deleted=0 ";
	statement << " ORDER BY start";

	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
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

Time_list Time_accessor::times_changed_since(time_t timestamp)
{
	Time_list result;

	Statement statement = database.prepare("SELECT taskID, start, stop, state, changed, uuid, id, task_UUID, comment FROM v_times WHERE changed>=?");

	statement.bind_value(1, timestamp);

	Query_result rows = statement.execute();
	for (vector<Data_cell> row : rows)
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

Time_id Time_accessor::uuid_to_id(UUID uuid)
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

bool Time_accessor::update(Time_entry item )
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

		database.send_notification(TASK_TIME_CHANGED, item.task_id);
		database.send_notification(TIME_ENTRY_CHANGED, item.id);

		return true;
	}
	return false;
}

Task_id_list Time_accessor::children_id_list(int64_t id)
{
	Task_id_list result;
	Statement statement_get_children_i_ds = database.prepare("SELECT id FROM tasks WHERE parent=?;");
	statement_get_children_i_ds.bind_value(1, id);
	Query_result rows = statement_get_children_i_ds.execute();

	for (auto row : rows)
	{
		result.push_back(row[0].integer());
	}
	return result;
}

Duration Time_accessor::total_cumulative_time(int64_t taskID, time_t start, time_t stop)
{
	Duration total_time = duration_time(taskID, start, stop);
	Task_id_list children = children_id_list(taskID);
	for (auto child : children)
	{
		total_time += total_cumulative_time(child, start, stop);
	}
	return total_time;
}

Time_id Time_accessor::create(Time_entry item)
{
	internal_create(item, statement_new_entry);
	Time_id time_id = database.id_of_last_insert();

	if (item.start != item.stop)
	{
		database.send_notification(TASK_UPDATED, item.task_id);
	}
	database.send_notification(TIME_ENTRY_CHANGED, time_id);
	return time_id;
}

void Time_accessor::internal_create(const Time_entry &item, Statement& statement_new_entry)
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

void Time_accessor::upgrade_to_db_5(Database& database)
{
	time_t now = time(nullptr);

	database.execute("DELETE FROM times WHERE taskID = 0");
	database.execute("DROP TABLE IF EXISTS times_backup");
	database.execute("ALTER TABLE times RENAME TO times_backup");
	Time_accessor::create_table(database);
	auto statement_new_entry(database.prepare(new_time_entry_statement));

	Statement statement = database.prepare("SELECT id, taskID, start, stop FROM  times_backup");
	Query_result rows = statement.execute();
	for (vector<Data_cell> row : rows)
	{
		int64_t id     = row[0].integer();
		int64_t task_id = row[1].integer();
		time_t  start  = row[2].integer();
		time_t  stop   = row[3].integer();

		Time_entry item(id, UUID(), task_id, {}, start, stop, STOPPED, now, "");
		internal_create(item, statement_new_entry);
	}
	database.execute("DROP TABLE IF EXISTS times_backup");
}
void Time_accessor::upgrade(Database& database)
{
	if (database.current_db_version() < 5 ) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		upgrade_to_db_5(database);
	}
	if(database.current_db_version() == 5) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (!database.column_exists("times", "state") )
		{
			database.execute(R"query(
				ALTER TABLE
					times
				ADD
					state INTEGER
				DEFAULT 0;
				)query");

			database.execute( R"query(
				UPDATE
					times
				SET
					state = 1
				WHERE
					running = 1;
				)query");

			database.execute( R"query(
				UPDATE
					times
				SET
					state = 3
				WHERE
					deleted = 1;
				)query");
		}
		if( ! database.column_exists("times", "comment") )
		{
			database.execute(
					R"query(
				ALTER TABLE
					times
				ADD
					comment VARCHAR;
				)query");
		}
	}
}

void Time_accessor::create_table(Database& database)
{
	database.execute( R"Query(
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

void Time_accessor::drop_views(Database& database)
{
	database.execute("DROP VIEW IF EXISTS v_times");
}
void Time_accessor::create_views(Database& database)
{
	Time_accessor::drop_views(database);
	database.execute(R"Query(
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

void Time_accessor::remove_short_time_spans()
{
	database.execute("DELETE FROM times WHERE stop-start < 60");
}


Task_id_list Time_accessor::active_tasks(time_t start, time_t stop)
{
	Task_id_list result_list;
	Statement statement_get_tasks = database.prepare(R"Query(
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

Time_list Time_accessor::by_state(Time_entry_state state) const
{
	Time_list time_list;

	Statement statement = database.prepare( R"Query(
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

void Time_accessor::setup(Database& database)
{
	Time_accessor::create_table(database);
	Time_accessor::drop_views(database);
	Time_accessor::upgrade(database);
	Time_accessor::create_views(database);
}

}

