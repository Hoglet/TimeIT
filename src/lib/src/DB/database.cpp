#include <iostream>
#include <sstream>

#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/database.h>

namespace libtimeit
{
using namespace std;

const int expectedDBVersion = 5;

void Database::create_tables( list<Accessor*>& accessors)
{
	db.execute("PRAGMA foreign_keys = ON");

	for ( auto accessor : accessors )
	{
		accessor->create_table();
	}

	//Create necessary tables

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
	int DBVersion;
	string statement = R"Query(
		SELECT
			value
		FROM
			parameters
		WHERE
			id == "dbversion"
		)Query";

	Query_result rows = db.execute(statement);
	if ( rows.empty() )
	{
		DBVersion = expectedDBVersion;
		statement = R"Query(
			INSERT INTO
				parameters (id, value)
			VALUES ( "dbversion"," )Query";
		statement += expectedDBVersion ;
		statement += R"end(");)end";
		db.execute(statement);
	}
	else
	{
		vector<Data_cell> row = rows.at(0);
		DBVersion = (int)row[0].integer();
	}
	return DBVersion;
}

void Database::upgrade( list<Accessor*>& accessors)
{
	int DBVersion = current_DB_version();
	if (DBVersion != expectedDBVersion)
	{
		for ( auto accessor : accessors)
		{
			accessor->upgrade();
		}
		stringstream statement;

		//Upgrade existing
		//LCOV_EXCL_START
		if (DBVersion < 4)
		{
			db.execute("DROP VIEW v_timesSummary;");
			db.execute("DROP VIEW v_tasks;");
		}
		//LCOV_EXCL_STOP

		statement.str("");
		statement << "UPDATE parameters SET value = ";
		statement << expectedDBVersion;
		statement << " WHERE id = \"dbversion\";";
		db.execute(statement.str());
	}
}

void Database::create_views( list<Accessor*>& accessors )
{
	//Create views
	db.execute("DROP VIEW IF EXISTS v_running");
	db.execute("CREATE VIEW v_running AS SELECT taskID, running FROM times WHERE running =\"1\"");
	db.execute("DROP VIEW IF EXISTS v_tasks");
	db.execute(
			"CREATE VIEW v_tasks AS SELECT tasks.*, IFNULL(v_running.running,0) as running FROM tasks    LEFT JOIN v_running    ON tasks.id=v_running.taskId");
	for ( auto accessor : accessors)
	{
		accessor->create_views();
	}
}

Database::Database(
		const std::string& dbname,
		Notifier&          publisher)
		:
		notifier(publisher),
		db(dbname)
{
	try
	{
		db.execute("PRAGMA foreign_keys = OFF");
		begin_transaction();

		Task_accessor      tasks( *this );
		Time_accessor      times( *this );
		Autotrack_accessor auto_track( *this );
		Settings_accessor  settings( *this );
		list<Accessor*> accessors =
				{
						&tasks,
						&times,
						&auto_track,
						&settings,
				};

		create_tables(accessors);
		upgrade(accessors);
		create_views( accessors);

		end_transaction();
		db.execute("PRAGMA foreign_keys = ON");

		times.remove_short_time_spans();
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
	notifier.enabled(state);
}


void Database::send_notification(message_type type, int64_t ID, string name)
{
	notifier.send_notification(type, ID, name);
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
