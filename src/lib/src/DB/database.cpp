#include <iostream>
#include <sstream>

#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <fmt/core.h>
#include "db_updater.h"

namespace libtimeit
{
using namespace std;

const int expected_db_version = 5;


int database::current_db_version() const
{
	return db_version;
}

database::database(
		const std::string& dbname,
		notification_manager&          publisher)
		:
		notifier(publisher),
		db(dbname)
{
	try
	{

		find_db_version();

		task_accessor::create_table(*this);
		time_accessor::create_table(*this);
		auto_track_accessor::create_table(*this);
		settings_accessor::create_table(*this);


		db_updater updater(*this);
		updater.upgrade();

		task_accessor::setup(*this);
		time_accessor::setup(*this);
		auto_track_accessor::setup(*this);
		settings_accessor::setup(*this);
		extended_task_accessor::setup(*this);



		begin_transaction();
		//db.execute("PRAGMA foreign_keys = OFF");

		time_accessor          times(*this );
		settings_accessor      settings(*this );





		db_version = expected_db_version;
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

bool database::table_exists(string name)
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
void database::find_db_version()
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
			vector<data_cell> row = rows.at(0);
			db_version = (int) row[0].integer();
		}
	}
	else if ( table_exists("settings") )
	{

		settings_accessor  settings(*this );
		auto version = settings.value("db_version");
		if (version.has_value())
		{
			db_version = version.value();
		}
	}
	else
	{
		db_version = expected_db_version;
	}
}

void database::begin_transaction()
{
	db.begin_transaction();
}

void database::try_rollback()
{
	db.try_rollback();
}

void database::end_transaction()
{
	db.end_transaction();
}
void database::enable_notifications(bool state)
{
	notifier.is_enabled(state);
}


void database::send_notification( message_type type, int64_t id, string name)
{
	notifier.try_send_notification( type, id, name);
}


query_result database::execute( const string& statement)
{
	return db.execute(statement);
}
sql_statement database::prepare(string statement)
{
	return db.prepare(statement);
}

int64_t database::id_of_last_insert()
{
	return db.id_of_last_insert();
}

[[nodiscard]] bool database::column_exists(string_view table, string_view  column)
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

void database::broadcast(function<void(event_observer*)> func) const
{
	notifier.broadcast(func);
}

}
