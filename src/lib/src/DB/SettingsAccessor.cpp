/*
 * Settings.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include <sstream>
#include <iostream>
#include <list>
#include <libtimeit/db/SettingsAccessor.h>

using namespace std;

namespace libtimeit
{

SettingsAccessor::SettingsAccessor(Database& op_database): database(op_database)
{
	idleGT = 7;
	idleGZ = 1;
	shortFilterTime = 1;
}

SettingsAccessor::~SettingsAccessor()
{
}

int SettingsAccessor::GetShortFilterTime()
{
	return shortFilterTime;
}
void SettingsAccessor::SetShortFilterTime(int value)
{
	shortFilterTime = value;
}

void SettingsAccessor::attach(SettingsAccessorObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}
void SettingsAccessor::detach(SettingsAccessorObserver* observer)
{
	if (observer)
	{
		observers.remove(observer);
	}
}

int SettingsAccessor::GetIntByName(const std::string& name, int defaultValue)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	int retVal = defaultValue;

	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		vector<DataCell> row = rows.at(0);
		retVal = row[0].getInt();
	}
	else
	{
		SetIntByName(name, defaultValue);
	}
	return retVal;
}

bool SettingsAccessor::SetIntByName(const std::string& name, int value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool retVal = false;

	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET intValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (intValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	return retVal;
}

bool SettingsAccessor::GetBoolByName(const std::string& name, bool defaultValue)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool retVal = defaultValue;

	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		vector<DataCell> row = rows.at(0);
		retVal = row[0].getBool();
	}
	else
	{
		SetBoolByName(name, defaultValue);
	}
	return retVal;
}

bool SettingsAccessor::SetBoolByName(const std::string& name, bool value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool retVal = false;
	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET boolValue = " << value;
		statement << " WHERE name = '" << name << "'";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (boolValue, name) ";
		statement << " VALUES (" << value << ", '" << name << "') ";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	return retVal;
}

std::string SettingsAccessor::GetStringByName(const std::string& name, const std::string& defaultValue)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	std::string retVal = defaultValue;

	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		vector<DataCell> row = rows.at(0);
		retVal = row[0].getString();
	}
	else
	{
		SetStringByName(name, defaultValue);
	}
	return retVal;
}

bool SettingsAccessor::SetStringByName(const std::string& name, const std::string& value)
{
	stringstream statement;
	statement << "SELECT stringValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool retVal = false;
	QueryResult rows = database.exe(statement.str());
	if (rows.size())
	{
		statement.str("");
		statement << "UPDATE settings";
		statement << " SET stringValue = '" << value << "'";
		statement << " WHERE name = '" << name << "'";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	else //Value did not exist
	{
		statement.str("");
		statement << "INSERT INTO settings";
		statement << " (stringValue, name) ";
		statement << " VALUES ('" << value << "', '" << name << "') ";
		database.exe(statement.str());
		SettingsChanged(name);
		retVal = true;
	}
	return retVal;
}


void SettingsAccessor::SettingsChanged(const std::string& name)
{
	for (SettingsAccessorObserver* observer : observers)
	{
		observer->on_settingsChanged(name);
	}
}
}
