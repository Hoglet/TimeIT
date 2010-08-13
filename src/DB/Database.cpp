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
	db.exe("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY, name VARCHAR, parent INTEGER, running BOOL DEFAULT false,expanded BOOL DEFAULT false , deleted BOOL DEFAULT false, autotrack BOOL DEFAULT false) ");
	db.exe("CREATE TABLE IF NOT EXISTS times (id INTEGER PRIMARY KEY, taskID INTEGER, start INTEGER, stop INTEGER, running BOOL DEFAULT false) ");
	db.exe("CREATE TABLE IF NOT EXISTS autotrack (taskID INTEGER, workspace INTEGER) ");
	db.exe("CREATE TABLE IF NOT EXISTS parameters (id VARCHAR PRIMARY KEY, string VARCHAR, value INTEGER, boolean BOOL) ");
	db.exe("CREATE TABLE IF NOT EXISTS settings (name VARCHAR PRIMARY KEY, intValue INTEGER, boolValue BOOL, stringValue VARCHAR) ");

	stringstream statement;
	statement << "SELECT value FROM parameters ";
	statement << " WHERE id == \"dbversion\"";


	//Do upgrade if necessary
	int DBVersion = 0;
	const int expectedDBVersion = 4;
	try
	{
		db.exe(statement.str());
		if (db.rows.size() > 0)
		{
			vector<DataCell> row = db.rows.at(0);
			DBVersion = row[0].getInt();
		}
		if (DBVersion != expectedDBVersion)
		{
			switch (DBVersion)
			{
			case 0: //Empty database. Populate with data
				statement.str("INSERT INTO parameters (id, value) VALUES ( \"dbversion\", 1 )");
				db.exe(statement.str());
				break;
			case 1:
				//Fall through
			case 2:
				//Fall through
			case 3:
				db.exe("DROP VIEW v_timesSummary;");
				db.exe("DROP VIEW v_tasks;");
			case 4:
				//Fall through
			default:
				break;
			}

			statement.str("");
			statement << "UPDATE parameters SET value = ";
			statement << expectedDBVersion;
			statement << " WHERE id = \"dbversion\";";
			db.exe(statement.str());
		}
	} catch (dbexception& e)
	{
		cerr << statement.str() << " caused: " << endl;
		cerr << e.what() << endl;
	}

	try
	{
		boost::shared_ptr<TaskAccessor> taskAccessor(new TaskAccessor(dbname));
		m_taskAccessor = taskAccessor;
		m_autotrackAccessor = boost::shared_ptr<AutotrackAccessor>(new AutotrackAccessor(dbname, taskAccessor));
		m_timeAccessor = boost::shared_ptr<TimeAccessor>(new TimeAccessor(dbname, taskAccessor));
		m_settingsAccessor = boost::shared_ptr<SettingsAccessor>(new SettingsAccessor(dbname));
	} catch (dbexception& e)
	{
		cerr << "Initiating DB with " << dbname << " caused: " << e.what() << endl;
		throw e;
	}

}

Database::~Database()
{
}

boost::shared_ptr<IAutotrackAccessor> Database::getAutotrackAccessor()
{
	return m_autotrackAccessor;
}

boost::shared_ptr<ITimeAccessor> Database::getTimeAccessor()
{
	return m_timeAccessor;
}

boost::shared_ptr<ITaskAccessor> Database::getTaskAccessor()
{
	return m_taskAccessor;
}

boost::shared_ptr<ISettingsAccessor> Database::getSettingsAccessor()
{
	return m_settingsAccessor;
}


}
