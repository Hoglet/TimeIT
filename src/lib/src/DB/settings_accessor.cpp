/*
 * Settings.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include <sstream>
#include <iostream>
#include <list>
#include <libtimeit/db/settings_accessor.h>

using namespace std;

namespace libtimeit
{

Settings_accessor::Settings_accessor(Database& op_database): database(op_database)
{
}

Settings_accessor::~Settings_accessor()
{
}

int Settings_accessor::get_int(string name, int default_value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	int return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].integer();
	}
	else
	{
		set_int(name, default_value);
	}
	return return_value;
}

bool Settings_accessor::set_int(string name, int value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = false;

	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET intValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (intValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	return return_value;
}

bool Settings_accessor::get_bool(string name, bool default_value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].boolean();
	}
	else
	{
		set_bool(name, default_value);
	}
	return return_value;
}

bool Settings_accessor::set_bool(string name, bool value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = false;
	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET boolValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (boolValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	return return_value;
}

string Settings_accessor::get_string(string name, string default_value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	string return_value = default_value;

	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		vector<Data_cell> row = rows.at(0);
		return_value = row[0].text();
	}
	else
	{
		set_string(name, default_value);
	}
	return return_value;
}

bool Settings_accessor::set_string(string name, string value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool return_value = false;
	Query_result rows = database.execute(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET stringValue = '" << value << "'";
		statement << " WHERE name = '" << name << "'";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (stringValue, name) ";
		statement << " VALUES ('" << value << "', '" << name << "') ";
		database.execute(statement.str());
		setting_changed(name);
		return_value = true;
	}
	return return_value;
}


void Settings_accessor::setting_changed(string name)
{
	database.send_notification(SETTINGS_CHANGED, 0, name);
}
}
