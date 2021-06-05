#include <iostream>
#include <sstream>

#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/database.h>
#include <fmt/core.h>

namespace libtimeit
{
using namespace std;

const int EXPECTED_DB_VERSION = 5;


int Database::current_db_version() const
{
	return db_version;
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


		Task_accessor::setup(*this);
		Time_accessor::setup(*this);
		Auto_track_accessor::setup(*this);
		Settings_accessor::setup(*this);
		Extended_task_accessor::setup(*this);

		begin_transaction();
		//db.execute("PRAGMA foreign_keys = OFF");

		Time_accessor          times( *this );
		Settings_accessor      settings( *this );





		db_version = EXPECTED_DB_VERSION;
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
	auto query = fmt::format(
			R"Query(
				SELECT
					name
				FROM
					sqlite_master
				WHERE
					type='table'
				AND
					name='{}')Query", name.c_str());
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
		db_version = EXPECTED_DB_VERSION;
	}
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
	notifier.is_enabled(state);
}


void Database::send_notification(message_type type, int64_t ID, string name)
{
	notifier.try_send_notification(type, ID, name);
}


Query_result Database::execute(const string& statement)
{
	return db.execute(statement);
}
Statement Database::prepare(string statement)
{
	return db.prepare(statement);
}

int64_t Database::id_of_last_insert()
{
	return db.id_of_last_insert();
}

[[nodiscard]] bool Database::column_exists( string_view table, string_view  column)
{
	auto query = fmt::format("SELECT {} FROM {}", column.data(), table.data() );
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
