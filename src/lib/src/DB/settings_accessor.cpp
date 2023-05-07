/*
 * Settings.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include <iostream>
#include <libtimeit/db/settings_accessor.h>
#include <optional>
#include <fmt/core.h>

using namespace std;

namespace libtimeit
{

settings_accessor::settings_accessor(database &op_database) : db(op_database)
{
}

optional<int> settings_accessor::value(string name)
{
	string statement = fmt::format(
			R"Query(
				SELECT
					intValue
				FROM
					settings
				WHERE
					name = '{}'; )Query", name.c_str());


	query_result rows = db.execute( statement);
	if (rows.empty())
	{
		return {};
	}
	vector<data_cell> row = rows.at(0);
	return row[0].integer();
}

int64_t settings_accessor::get_int(string name, int64_t default_value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	auto return_value = default_value;

	query_result rows = db.execute( statement.str());
	if (rows.empty())
	{
		set_int(name, default_value);
	}
	else
	{
		vector<data_cell> row = rows.at(0);
		return_value = row[0].integer();
	}
	return return_value;
}

bool settings_accessor::set_int(string name, int64_t value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";


	auto rows = db.execute(statement.str());
	if (rows.empty())
	{//Value did not exist
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (intValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		db.execute(statement.str());
		setting_changed(name);
	}
	else
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET intValue = " << value;
		statement << " WHERE name = '" << name << "'";
		db.execute(statement.str());
		setting_changed(name);
	}
	return true;
}

bool settings_accessor::get_bool(string name, bool default_value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = default_value;

	query_result rows = db.execute( statement.str());
	if (rows.empty())
	{
		set_bool(name, default_value);
	}
	else
	{
		vector<data_cell> row = rows.at(0);
		return_value = row[0].boolean();
	}
	return return_value;
}

bool settings_accessor::set_bool(string name, bool value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	query_result rows = db.execute( statement.str());
	if (rows.empty())
	{//Value did not exist
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (boolValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		db.execute(statement.str());
		setting_changed(name);
	}
	else
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET boolValue = " << value;
		statement << " WHERE name = '" << name << "'";
		db.execute(statement.str());
		setting_changed(name);

	}
	return true;
}

string settings_accessor::get_string(string_view name, string_view default_value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";


	query_result rows = db.execute( statement.str());
	if (rows.empty())
	{
		set_string(name, default_value);
	}
	else
	{
		vector<data_cell> row = rows.at(0);
		return row[0].text();
	}
	return string(default_value);
}

bool settings_accessor::set_string(string_view name, string_view value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	query_result rows = db.execute( statement.str());
	if (rows.empty())
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (stringValue, name) ";
		statement << " VALUES ('" << value << "', '" << name << "') ";
		db.execute(statement.str());
		setting_changed(name);
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET stringValue = '" << value << "'";
		statement << " WHERE name = '" << name << "'";
		db.execute(statement.str());
		setting_changed(name);
	}
	return true;
}


void settings_accessor::setting_changed(string_view name)
{
	db.send_notification( settings_changed, 0, name);
}

void settings_accessor::create_table(database& db)
{
	db.execute(R"Query(
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

void settings_accessor::upgrade(database& /*database*/)
{
	//noop
}

void settings_accessor::setup(database& db)
{
	settings_accessor::create_table(db);
	settings_accessor::upgrade(db );
}

}
