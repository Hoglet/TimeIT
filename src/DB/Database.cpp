/*
 * Database.cpp
 *
 *  Created on: Mar 3, 2009
 *      Author: hoglet
 */

#include "internal/TimeAccessor.h"
#include "internal/AutotrackAccessor.h"
#include "internal/TaskAccessor.h"
#include "internal/SettingsAccessor.h"
#include <iostream>
#include <CSQL.h>
#include <Database.h>
#include <sstream>

namespace DB
{
using namespace std;
using namespace DBAbstraction;
Database::Database(const std::string& dbname)
{

	CSQL db(dbname);

	//Create necessary tables
	db.exe(
			"CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY, name VARCHAR, parent INTEGER, running BOOL DEFAULT false,expanded BOOL DEFAULT false , deleted BOOL DEFAULT false) ");
	db.exe(
			"CREATE TABLE IF NOT EXISTS times (id INTEGER PRIMARY KEY, taskID INTEGER, start INTEGER, stop INTEGER, running BOOL DEFAULT false) ");
	db.exe("CREATE TABLE IF NOT EXISTS autotrack (taskID INTEGER, workspace INTEGER) ");
	db.exe(
			"CREATE TABLE IF NOT EXISTS parameters (id VARCHAR PRIMARY KEY, string VARCHAR, value INTEGER, boolean BOOL) ");
	db.exe(
			"CREATE TABLE IF NOT EXISTS settings (name VARCHAR PRIMARY KEY, intValue INTEGER, boolValue BOOL, stringValue VARCHAR) ");

	stringstream statement;
	statement << "SELECT value FROM parameters ";
	statement << " WHERE id == \"dbversion\"";

	//Do upgrade if necessary
	int DBVersion = 0;
	const int expectedDBVersion = 4;

	db.exe(statement.str());
	if (db.rows.size() > 0)
	{
		vector<DataCell> row = db.rows.at(0);
		DBVersion = row[0].getInt();
	}
	if (DBVersion != expectedDBVersion)
	{
		if (DBVersion == 0)
		{ //Empty database. Populate with data
			statement.str("INSERT INTO parameters (id, value) VALUES ( \"dbversion\", 1 )");
			db.exe(statement.str());
		}
		else
		{ //Upgrade existing
			if (DBVersion < 4)
			{
				db.exe("DROP VIEW v_timesSummary;");
				db.exe("DROP VIEW v_tasks;");
			}
		}

		statement.str("");
		statement << "UPDATE parameters SET value = ";
		statement << expectedDBVersion;
		statement << " WHERE id = \"dbversion\";";
		db.exe(statement.str());
	}

	try
	{
		std::shared_ptr<Notifier> notifier(new Notifier());
		m_notifier = notifier;
		m_timeAccessor = std::shared_ptr<TimeAccessor>(new TimeAccessor(dbname, notifier));
		m_taskAccessor = std::shared_ptr<TaskAccessor>(new TaskAccessor(dbname, notifier, m_timeAccessor));
		m_autotrackAccessor = std::shared_ptr<AutotrackAccessor>(new AutotrackAccessor(dbname, m_taskAccessor));
		m_settingsAccessor = std::shared_ptr<SettingsAccessor>(new SettingsAccessor(dbname));
	} catch (dbexception& e)
	{
		cerr << "Initiating DB with " << dbname << " caused: " << e.what() << endl;
		throw e;
	}

}

Database::~Database()
{
}

std::shared_ptr<IAutotrackAccessor> Database::getAutotrackAccessor()
{
	return m_autotrackAccessor;
}

std::shared_ptr<ITimeAccessor> Database::getTimeAccessor()
{
	return m_timeAccessor;
}

std::shared_ptr<ITaskAccessor> Database::getTaskAccessor()
{
	return m_taskAccessor;
}

std::shared_ptr<ISettingsAccessor> Database::getSettingsAccessor()
{
	return m_settingsAccessor;
}

}
