#include <iostream>
#include <sstream>

#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/database.h>
#include <libtimeit/utils.h>

namespace libtimeit
{
using namespace std;

const int expected_DB_version = 5;

void Database::create_tables( list<Accessor*>& accessors)
{
	for ( auto *accessor : accessors )
	{
		accessor->create_table();
	}
}



int Database::current_DB_version() const
{
	return db_version;
}

void Database::upgrade( list<Accessor*>& accessors)
{
	for ( auto *accessor : accessors)
	{
		accessor->upgrade();
	}

	//LCOV_EXCL_START
	db.execute("DROP VIEW IF EXISTS v_timesSummary;");
	//LCOV_EXCL_STOP
}

void Database::drop_views( list<Accessor*>& accessors )
{
	for ( auto *accessor : accessors)
	{
		accessor->drop_views();
	}
}

void Database::create_views( list<Accessor*>& accessors )
{
	for ( auto *accessor : accessors)
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

		find_db_version();

		begin_transaction();

		Task_accessor          tasks( *this );
		Time_accessor          times( *this );
		Auto_track_accessor    auto_track(*this );
		Settings_accessor      settings( *this );
		Extended_task_accessor extended_tasks( *this);

		list<Accessor*> accessors =
				{
						&tasks,
						&times,
						&auto_track,
						&settings,
						&extended_tasks,
				};

		create_tables(accessors);
		db.execute("PRAGMA foreign_keys = OFF");
		drop_views( accessors );
		upgrade(accessors);
		create_views( accessors );

		db_version = expected_DB_version;
		settings.set_int("db_version", db_version);
		db.execute( "DROP TABLE IF EXISTS parameters;" );

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

bool Database::table_exists(string name)
{
	auto query = string_printf(
			R"Query(
				SELECT
					name
				FROM
					sqlite_master
				WHERE
					type='table'
				AND
					name='%s')Query", name.c_str());
	auto rows = db.execute( query );

	return !rows.empty();


}
void Database::find_db_version()
{

	if ( table_exists("parameters") )
	{
		auto rows = db.execute(
				R"Query(
				SELECT
					value
				FROM
					parameters
				WHERE
				id == "dbversion"
			)Query");

		if (!rows.empty())
		{
			vector<Data_cell> row = rows.at(0);
			db_version = (int) row[0].integer();
		}
	}
	else if ( table_exists("settings") )
	{

		Settings_accessor  settings( *this );
		auto version = settings.value("db_version");
		if (version.has_value())
		{
			db_version = version.value();
		}
	}
	else
	{
		db_version = expected_DB_version;
	}
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
	return db.prepare(statement);
}

int64_t Database::ID_of_last_insert()
{
	return db.ID_of_last_insert();
}

[[nodiscard]] bool Database::column_exists( string_view table, string_view  column)
{
	auto query = string_printf("SELECT %s FROM %s", column.data(), table.data() );
	try
	{
		db.execute(query);
		return true;
	}
	catch (exception& e)
	{
		return false;
	}
}

}
