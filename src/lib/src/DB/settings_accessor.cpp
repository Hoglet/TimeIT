/*
 * Settings.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include <sstream>
#include <iostream>
#include <libtimeit/db/settings_accessor.h>
#include <optional>
#include <fmt/core.h>

using namespace std;

namespace libtimeit
{

Settings_accessor::Settings_accessor(Database &op_database) : database(op_database)
{
}

optional<int> Settings_accessor::value(string name)
{
	string statement = fmt::format(
			R"Query(
				SELECT
					intValue
				FROM
					settings
				WHERE
					name = '{}'; )Query", name.c_str());


	Query_result rows = database.execute(statement);
	if (rows.empty())
	{
		return {};
	}
	vector<Data_cell> row = rows.at(0);
	return row[0].integer();
}

int64_t Settings_accessor::get_int(string name, int64_t default_value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	auto return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.empty())
	{
		set_int(name, default_value);
	}
	else
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].integer();
	}
	return return_value;
}

bool Settings_accessor::set_int(string name, int64_t value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";


	auto rows = database.execute(statement.str());
	if (rows.empty())
	{//Value did not exist
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (intValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
	}
	else
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET intValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);
	}
	return true;
}

bool Settings_accessor::get_bool(string name, bool default_value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.empty())
	{
		set_bool(name, default_value);
	}
	else
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].boolean();
	}
	return return_value;
}

bool Settings_accessor::set_bool(string name, bool value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	Query_result rows = database.execute(statement.str());
	if (rows.empty())
	{//Value did not exist
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (boolValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
	}
	else
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET boolValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);

	}
	return true;
}

string Settings_accessor::get_string(string name, string default_value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	string return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.empty())
	{
		set_string(name, default_value);
	}
	else
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].text();
	}
	return return_value;
}

bool Settings_accessor::set_string(string name, string value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	Query_result rows = database.execute(statement.str());
	if (rows.empty())
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (stringValue, name) ";
		statement << " VALUES ('" << value << "', '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET stringValue = '" << value << "'";
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);
	}
	return true;
}


void Settings_accessor::setting_changed(string name)
{
	database.send_notification(SETTINGS_CHANGED, 0, name);
}

void Settings_accessor::create_table(Database& database)
{
	database.execute( R"Query(
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

void Settings_accessor::upgrade(Database& /*database*/)
{
	//noop
}

void Settings_accessor::setup(Database& database)
{
	Settings_accessor::create_table(database);
	Settings_accessor::upgrade( database );
}

}