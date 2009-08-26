/*
 * Settings.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include <sstream>
#include <iostream>
#include <list>
#include "SettingsAccessor.h"

using namespace std;
using namespace DBAbstraction;
SettingsAccessor::SettingsAccessor(const std::string& dbpath): db(dbpath)
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

void SettingsAccessor::attach(ISettingsAccessorObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}
void SettingsAccessor::detach(ISettingsAccessorObserver* observer)
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
	try
	{
		db.exe(statement.str());
		if (db.rows.size())
		{
			vector<DataCell> row = db.rows[0];
			retVal = row[0].getInt();
		}
		else
		{
			SetIntByName( name,  defaultValue);
		}
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return retVal;
}

bool SettingsAccessor::SetIntByName(const std::string& name, int value)
{
	stringstream statement;
	statement << "SELECT intValue FROM settings ";
	statement << " WHERE name = '" << name <<"'";

	bool retVal = false;
	try
	{
		db.exe(statement.str());
		if (db.rows.size())
		{
			statement.str("");
			statement << "UPDATE settings";
			statement << " SET intValue = " << value;
			statement << " WHERE name = '" << name <<"'";
			db.exe(statement.str());
			SettingsChanged(name);
			retVal = true;
		}
		else //Value did not exist

		{
			statement.str("");
			statement << "INSERT INTO settings";
			statement << " (intValue, name) ";
			statement << " VALUES (" << value << ", '" << name << "') ";
			db.exe(statement.str());
			SettingsChanged(name);
			retVal = true;
		}
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return retVal;
}

bool SettingsAccessor::GetBoolByName(const std::string& name, bool defaultValue)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name << "'";

	bool retVal = defaultValue;
	try
	{
		db.exe(statement.str());
		if (db.rows.size())
		{
			vector<DataCell> row = db.rows[0];
			retVal = row[0].getBool();
		}
		else
		{
			SetBoolByName( name,  defaultValue);
		}
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return retVal;
}

bool SettingsAccessor::SetBoolByName(const std::string& name, bool value)
{
	stringstream statement;
	statement << "SELECT boolValue FROM settings ";
	statement << " WHERE name = '" << name <<"'";

	bool retVal = false;
	try
	{
		db.exe(statement.str());
		if (db.rows.size())
		{
			statement.str("");
			statement << "UPDATE settings";
			statement << " SET boolValue = " << value;
			statement << " WHERE name = '" << name <<"'";
			db.exe(statement.str());
			SettingsChanged(name);
			retVal = true;
		}
		else //Value did not exist

		{
			statement.str("");
			statement << "INSERT INTO settings";
			statement << " (boolValue, name) ";
			statement << " VALUES (" << value << ", '" << name << "') ";
			db.exe(statement.str());
			SettingsChanged(name);
			retVal = true;
		}
	} catch (dbexception& e)
	{
		cerr << statement << " caused: " << endl;
		cerr << e.what() << endl;
	}
	return retVal;
}

void SettingsAccessor::SettingsChanged(const std::string& name)
{
	try
	{
		//		Task task = getTask(taskID);
		std::list<ISettingsAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			ISettingsAccessorObserver* observer = *iter;
			observer->on_settingsChanged(name);
		}
	} catch (...)
	{
	}
}
