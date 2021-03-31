#include <iostream>
#include <sstream>

#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/database.h>

namespace libtimeit
{
using namespace std;

void Database::create_tables()
{
	db.execute("PRAGMA foreign_keys = ON");
	//Create necessary tables
	Time_accessor time_accessor(*this);
	Task_accessor task_accessor(*this);
	time_accessor.create_table();
	task_accessor.create_table();
	db.execute(R"Query(
		CREATE TABLE IF NOT EXISTS
			autotrack
			(
				taskID    INTEGER,
				workspace INTEGER
			)
		)Query");

	db.execute( R"Query(
		CREATE TABLE IF NOT EXISTS
			parameters
			(
				id       VARCHAR PRIMARY KEY,
				string   VARCHAR,
				value    INTEGER,
				boolean  BOOL
			)
		)Query");
	db.execute( R"Query(
		CREATE TABLE IF NOT EXISTS
			settings
			(
				name        VARCHAR PRIMARY KEY,
				intValue    INTEGER,
				boolValue   BOOL,
				stringValue VARCHAR
			)
		)Query");
}



int Database::current_DB_version()
{
	int DBVersion = 0;
	string statement = R"Query(
		SELECT
			value
		FROM
			parameters
		WHERE
			id == "dbversion"
		)Query";

	//Do upgrade if necessary
	Query_result rows = db.execute(statement);
	if (rows.size() > 0)
	{
		vector<Data_cell> row = rows.at(0);
		DBVersion = row[0].integer();
	}
	return DBVersion;
}

void Database::upgrade_if_needed(int DBVersion, const int expectedDBVersion)
{
	if (DBVersion != expectedDBVersion)
	{
		stringstream statement;
		if (DBVersion == 0)
		{
			//Empty database. Populate with data
			statement.str("INSERT INTO parameters (id, value) VALUES ( \"dbversion\", 1 )");
			db.execute(statement.str());
		}
		else
		{
			//Upgrade existing
			//LCOV_EXCL_START
			if (DBVersion < 4)
			{
				db.execute("DROP VIEW v_timesSummary;");
				db.execute("DROP VIEW v_tasks;");
			}
			//LCOV_EXCL_STOP
			if (DBVersion < 5)
			{
				Task_accessor task_accessor(*this);
				Time_accessor time_accessor(*this);
				task_accessor.upgrade_to_DB5();
				time_accessor.upgrade_to_DB5();
			}
		}
		statement.str("");
		statement << "UPDATE parameters SET value = ";
		statement << expectedDBVersion;
		statement << " WHERE id = \"dbversion\";";
		db.execute(statement.str());
	}
}

void Database::createViews()
{
	//Create views
	db.execute("DROP VIEW IF EXISTS v_running");
	db.execute("CREATE VIEW v_running AS SELECT taskID, running FROM times WHERE running =\"1\"");
	db.execute("DROP VIEW IF EXISTS v_tasks");
	db.execute(
			"CREATE VIEW v_tasks AS SELECT tasks.*, IFNULL(v_running.running,0) as running FROM tasks    LEFT JOIN v_running    ON tasks.id=v_running.taskId");
	Time_accessor time_accessor(*this);
	time_accessor.create_views();
}

Database::Database(
		const std::string& dbname,
		Notifier&          notifier)
		:
		m_notifier(notifier),
		db(dbname)
{
	try
	{
		db.execute("PRAGMA foreign_keys = OFF");
		begin_transaction();

		create_tables();

		const int expectedDBVersion = 5;

		int DBVersion = current_DB_version();
		upgrade_if_needed(DBVersion, expectedDBVersion);
		createViews();

		end_transaction();
		db.execute("PRAGMA foreign_keys = ON");

		Time_accessor time_accessor(*this);
		time_accessor.remove_short_time_spans();
	//LCOV_EXCL_START
	} catch (db_exception& e)
	{
		try_rollback();
		cerr << "Initiating DB with " << dbname << " caused: " << e.what() << endl;
		throw;
	}
	//LCOV_EXCL_STOP

}

Database::~Database()
{
}

void Database::begin_transaction()
{
	db.begin_transaction();
}

void Database::try_rollback()
{
	db.try_rollback();
}

void Database::end_transaction()
{
	db.end_transaction();
}
void Database::enable_notifications(bool state)
{
	m_notifier.enabled(state);
}

void Database::send_notification(message_type type, int64_t taskId)
{
	m_notifier.send_notification(type, taskId);
}


Query_result Database::execute(const string& statement)
{
	return db.execute(statement);
}
Statement Database::prepare(string statement)
{
	return db.prepare(statement.c_str());
}

int64_t Database::ID_of_last_insert()
{
	return db.ID_of_last_insert();
}

}
