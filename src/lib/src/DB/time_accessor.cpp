#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-magic-numbers"
#include <iostream>
#include <sstream>
#include <string>
#include <libtimeit/db/time_accessor.h>

using namespace std;
namespace libtimeit
{

Time_accessor::Time_accessor(Database &op_database) : database(op_database)
{
}



void Time_accessor::stop_all()
{
	database.execute("UPDATE times SET running = 0");
}


void Time_accessor::remove(int64_t id)
{
	auto te = by_ID(id);
	if(te)
	{
		update(te->with_deleted(true));
	}
}

optional<Time_entry> Time_accessor::by_ID(int64_t id)
{
	Statement statement = database.prepare(
			"SELECT taskID, start, stop, running, changed, deleted, uuid, task_UUID FROM v_times WHERE id = ?");
	statement.bind_value(1, id);

	Query_result rows = statement.execute();
	if (rows.size() > 0)
	{
		vector<Data_cell> row = rows.at(0);
		int     taskID   = row[0].integer();
		time_t  start    = row[1].integer();
		time_t  stop     = row[2].integer();
		bool    running  = row[3].boolean();
		int64_t changed  = row[4].integer();
		bool    deleted  = row[5].boolean();
		auto    uuid     = UUID::from_string(row[6].text());
		auto    taskUUID = UUID::from_string(row[7].text());

		Time_entry_state state = STOPPED;
		if ( running )
		{
			state = RUNNING;
		}
		if ( deleted )
		{
			state = DELETED;
		}


		if( uuid && taskUUID )
		{

			return Time_entry(id, *uuid, taskID, *taskUUID, start, stop, deleted, state, changed);
		}
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
		Statement statement_timeCompletelyWithinLimits = database.prepare("SELECT SUM(stop-start) AS time "
				" FROM times  WHERE taskID = ? AND start>=? AND stop<=? and deleted=0;");
		statement_timeCompletelyWithinLimits.bind_value(1, taskID);
		statement_timeCompletelyWithinLimits.bind_value(2, start);
		statement_timeCompletelyWithinLimits.bind_value(3, stop);
		rows = statement_timeCompletelyWithinLimits.execute();
	}
	else
	{
		Statement statement_getTime = database.prepare("SELECT SUM(stop-start) AS time  FROM times WHERE taskID = ? AND deleted=0;");
		statement_getTime.bind_value(1, taskID);
		rows = statement_getTime.execute();
	}
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = row[0].integer();
		}
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
	statement << " AND deleted=0 ";
	Query_result rows = database.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = row[0].integer();
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
	statement << " AND deleted=0 ";
	Query_result rows = database.execute(statement.str());
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		if (row[0].has_value())
		{
			time = row[0].integer();
		}
	}
	return time;
}

Task_ID_list Time_accessor::latest_active_tasks(int amount)
{
	Task_ID_list resultList;
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
		Task_ID id = row[0].integer();
		resultList.push_back(id);
	}
	return resultList;

}

Task_ID_list Time_accessor::currently_running()
{
	Task_ID_list resultList;
	Statement statement_getRunningTasks = database.prepare(
			"SELECT DISTINCT times.taskid FROM times WHERE times.running;");

	Query_result rows = statement_getRunningTasks.execute();

	for (auto row : rows)
	{
		int id = row[0].integer();
		resultList.push_back(id);
	}
	return resultList;
}

Time_list Time_accessor::time_list(int64_t taskId, time_t startTime, time_t stopTime)
{
	Time_list resultList;
	stringstream statement;
	statement << "SELECT id, start, stop, deleted, running, changed, uuid, task_UUID FROM v_times ";
	statement << " WHERE stop > " << startTime;
	statement << " AND start <" << stopTime;
	statement << " AND taskID = " << taskId;
	statement << " AND deleted=0 ";
	statement << " ORDER BY start";

	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
	{
		int64_t id = row[0].integer();
		time_t start = row[1].integer();
		time_t stop = row[2].integer();
		bool deleted = row[3].boolean();
		bool running = row[4].boolean();
		time_t changed = row[5].integer();
		auto uuid = UUID::from_string(row[6].text());
		auto taskUUID = UUID::from_string(row[7].text());

		Time_entry_state state = STOPPED;
		if ( running )
		{
			state = RUNNING;
		}
		if ( deleted )
		{
			state = DELETED;
		}

		if (uuid && taskUUID)
		{
			resultList.push_back(Time_entry(id, *uuid, taskId, *taskUUID, start, stop, deleted, state, changed));
		}
	}
	return resultList;
}

void Time_accessor::setRunning(int64_t timeID, bool running)
{
	auto te = by_ID(timeID);
	if(te)
	{
		int64_t taskID = te->task_ID;
		stringstream statement;
		statement << "UPDATE times SET running = " << (int) running;
		statement << " WHERE id=" << timeID;

		database.execute(statement.str());
		database.send_notification(TASK_UPDATED, taskID);
	}
}

Time_list Time_accessor::times_changed_since(time_t timestamp)
{
	Time_list result;

	Statement statement = database.prepare("SELECT taskID, start, stop, running, changed, deleted, uuid, id, task_UUID FROM v_times WHERE changed>=?");

	statement.bind_value(1, timestamp);

	Query_result rows = statement.execute();
	for (vector<Data_cell> row : rows)
	{
		int taskID = row[0].integer();
		time_t start = row[1].integer();
		time_t stop = row[2].integer();
		bool running = row[3].boolean();
		time_t changed = row[4].integer();
		bool deleted = row[5].boolean();
		auto uuid = UUID::from_string(row[6].text());
		int64_t id = row[7].integer();
		auto taskUUID = UUID::from_string(row[8].text());

		Time_entry_state state = STOPPED;
		if ( running )
		{
			state = RUNNING;
		}
		if ( deleted )
		{
			state = DELETED;
		}

		if(uuid && taskUUID)
		{
			Time_entry te(id, *uuid, taskID, *taskUUID, start, stop, deleted, state, changed);
			result.push_back(te);
		}
	}
	return result;
}

Time_ID Time_accessor::uuid_to_id(UUID uuid)
{
	Time_ID id = 0;
	Statement statement_uuidToId = database.prepare("SELECT id FROM times WHERE uuid=?;");
	statement_uuidToId.bind_value(1, uuid.c_str());
	Query_result rows = statement_uuidToId.execute();
	for (auto row : rows)
	{
		id = row[0].integer();
	}
	return id;
}

bool Time_accessor::update(Time_entry item )
{
	int64_t id = item.ID;
	auto existingItem = by_ID(id);
	if (existingItem && item != *existingItem && item.changed >= existingItem->changed)
	{
		Statement statement_updateTime = database.prepare(
				"UPDATE times SET uuid=?, taskID=?, start=?, stop=?, running=?, changed=?, deleted=? WHERE id=?");
		statement_updateTime.bind_value(1, item.uuid.c_str());
		statement_updateTime.bind_value(2, item.task_ID);
		statement_updateTime.bind_value(3, item.start);
		statement_updateTime.bind_value(4, item.stop);
		statement_updateTime.bind_value(5, item.running());
		statement_updateTime.bind_value(6, item.changed);
		statement_updateTime.bind_value(7, item.deleted);
		statement_updateTime.bind_value(8, item.ID);

		statement_updateTime.execute();

		database.send_notification(TASK_TIME_CHANGED, item.task_ID);
		database.send_notification(TIME_ENTRY_CHANGED, item.ID);

		return true;
	}
	return false;
}

Task_ID_list Time_accessor::children_IDs(int64_t taskID)
{
	Task_ID_list result;
	Statement statement_getChildrenIDs = database.prepare("SELECT id FROM tasks WHERE parent=?;");
	statement_getChildrenIDs.bind_value(1, taskID);
	Query_result rows = statement_getChildrenIDs.execute();

	for (auto row : rows)
	{
		int64_t id = row[0].integer();
		result.push_back(id);
	}
	return result;
}

Duration Time_accessor::total_cumulative_time(int64_t taskID, time_t start, time_t stop)
{
	Duration totalTime = duration_time(taskID, start, stop);
	Task_ID_list children = children_IDs(taskID);
	for (auto child : children)
	{
		totalTime += total_cumulative_time(child, start, stop);
	}
	return totalTime;
}

Time_ID Time_accessor::create(Time_entry item)
{
	Statement statement_newEntry = database.prepare(
			"INSERT INTO times (uuid,taskID, start, stop, changed,deleted) VALUES (?,?,?,?,?,?)");
	statement_newEntry.bind_value(1, item.uuid.c_str());
	statement_newEntry.bind_value(2, item.task_ID);
	statement_newEntry.bind_value(3, item.start);
	statement_newEntry.bind_value(4, item.stop);
	statement_newEntry.bind_value(5, item.changed);
	statement_newEntry.bind_value(6, item.deleted);
	statement_newEntry.execute();
	Time_ID time_ID = database.ID_of_last_insert();

	if (item.start != item.stop)
	{
		database.send_notification(TASK_UPDATED, item.task_ID);
	}
	return time_ID;
}

void Time_accessor::upgrade_to_DB5()
{
	time_t now = time(nullptr);

	database.execute("DELETE FROM times WHERE taskID = 0");
	database.execute("DROP TABLE IF EXISTS times_backup");
	database.execute("ALTER TABLE times RENAME TO times_backup");
	create_table();

	Statement statement = database.prepare("SELECT id, taskID, start, stop FROM  times_backup");
	Query_result rows = statement.execute();
	for (vector<Data_cell> row : rows)
	{
		int64_t id     = row[0].integer();
		int64_t taskID = row[1].integer();
		time_t  start  = row[2].integer();
		time_t  stop   = row[3].integer();

		Time_entry item(id, UUID(), taskID, {}, start, stop, false, STOPPED, now);
		create(item);
	}
	database.execute("DROP TABLE IF EXISTS times_backup");
}
void Time_accessor::upgrade()
{
	if ( database.current_DB_version() <5 )
	{
		upgrade_to_DB5();
	}
}

void Time_accessor::create_table()
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
			 FOREIGN KEY(taskID) REFERENCES tasks(id)
			)
		)Query"
		);
}

void Time_accessor::drop_views()
{
	database.execute("DROP VIEW IF EXISTS v_times");
}
void Time_accessor::create_views()
{

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


Task_ID_list Time_accessor::active_tasks(time_t start, time_t stop)
{
	Task_ID_list resultList;
	Statement statement_getTasks = database.prepare( R"Query(
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
						times.deleted=0;
					)Query"
					);
	statement_getTasks.bind_value(1, start);
	statement_getTasks.bind_value(2, stop);
	statement_getTasks.bind_value(3, start);
	statement_getTasks.bind_value(4, stop);
	statement_getTasks.bind_value(5, start);
	statement_getTasks.bind_value(6, stop);

	Query_result rows = statement_getTasks.execute();

	for (auto row : rows)
	{
		int id = row[0].integer();
		resultList.push_back(id);
	}
	return resultList;
}

}

#pragma clang diagnostic pop