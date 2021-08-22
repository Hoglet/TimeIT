#include "db_updater.h"

namespace libtimeit
{

static const string TIME_SCHEMA_V5 = R"Query(
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
		)Query";

static const string TASK_SCHEMA_V5 = R"Query(
		CREATE TABLE IF NOT EXISTS
 			tasks
			 (
				id          INTEGER PRIMARY KEY,
				name        VARCHAR,
				parent      INTEGER,
				changed     INTEGER,
				uuid        TEXT UNIQUE,
				expanded    BOOL DEFAULT    0,
				deleted     BOOL DEFAULT    0,
			 	completed   BOOL DEFAULT    0,
			 FOREIGN KEY(parent) REFERENCES tasks(id)
			)
			)Query";


static const string CTEATE_TIME_ENTRY_V5 = R"Query(
	INSERT INTO
		times (uuid,taskID, start, stop, changed,deleted,running)
	VALUES (?,?,?,?,?,0,0))Query";

const string CREATE_TASK_V5 = R"Query(
				INSERT INTO
					tasks (id,name,parent,changed,uuid,completed,deleted)
				VALUES    (?, ?, ?, ?, ?, 0, 0);
				)Query";


void db_updater::upgrade_to_db_5()
{
	update_tasks_to_db_5();
	update_times_to_db_5();
}

void db_updater::upgrade()
{
	if (db.current_db_version() < 5) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		upgrade_to_db_5();
	}
	upgrade_to_db_6();
}


void db_updater::update_times_to_db_5()
{
	static const int CHANGED_INDEX = 5;

	time_t now = time(nullptr);

	db.execute("DELETE FROM times WHERE taskID = 0");
	db.execute("DROP TABLE IF EXISTS times_backup");
	db.execute("ALTER TABLE times RENAME TO times_backup");
	db.prepare(TIME_SCHEMA_V5).execute();
	auto statement_new_entry(db.prepare(CTEATE_TIME_ENTRY_V5));

	sql_statement statement = db.prepare("SELECT  taskID, start, stop FROM  times_backup");
	Query_result rows = statement.execute();
	statement_new_entry.bind_value( CHANGED_INDEX, now);

	for (vector<data_cell> row : rows)
	{
		statement_new_entry.bind_value(1, static_cast<string>(time_id()));
		statement_new_entry.bind_value(2, row[0].integer() ); // owner id
		statement_new_entry.bind_value( 3, row[1].integer()); // start
		statement_new_entry.bind_value( 4, row[2].integer()); // stop
		statement_new_entry.execute();
	}
	db.execute("DROP TABLE IF EXISTS times_backup");
}

void db_updater::update_tasks_to_db_5()
{
	time_t now = time(nullptr);
//Update Tasks to new design
	db.execute("ALTER TABLE tasks RENAME TO tasks_backup");
	db.prepare(TASK_SCHEMA_V5).execute();
	auto statement_new_task(db.prepare(CREATE_TASK_V5 ));

	db.execute("UPDATE tasks_backup SET deleted = 0 WHERE deleted != 1");
	db.execute("UPDATE tasks_backup SET parent = NULL WHERE parent = 0");

	statement_new_task.bind_value(4, now);

	Query_result rows = db.prepare("SELECT id, name, parent, deleted FROM  tasks_backup").execute();
	for (vector<data_cell> row : rows)
	{
		statement_new_task.bind_value(1, row[0].integer());
		statement_new_task.bind_value(2, row[1].text());
		int64_t parent = 0;
		if (row[2].has_value())
		{
			parent = row[2].integer();
		}
		statement_new_task.bind_value( 3, parent);
		statement_new_task.bind_value( 4, now);
		statement_new_task.bind_value( 5, static_cast<string>(uuid()));
		statement_new_task.bind_value( 6, false);
		statement_new_task.bind_value( 7, row[3].boolean());
		statement_new_task.execute();
	}
	db.execute("DROP TABLE tasks_backup");
}

void db_updater::upgrade_to_db_6()
{
	upgrade_tasks_to_db_6();
	upgrade_times_to_db_6();
}

void db_updater::upgrade_tasks_to_db_6()
{
	if( ! db.column_exists("tasks", "idle") )
	{
		const auto *query = R"Query(
			ALTER TABLE
				tasks
			ADD
				idle        INTEGER
			DEFAULT 0
				;
			)Query";
		db.execute(query );

		const auto *query2 = R"Query(
			ALTER TABLE
				tasks
			ADD
				quiet      BOOL
			DEFAULT 0
				;
			)Query";
		db.execute(query2 );
	}
}

void db_updater::upgrade_times_to_db_6()
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