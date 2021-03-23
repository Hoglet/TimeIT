#include <iostream>
#include <sstream>

#include <libtimeit/db/TimeAccessor.h>
#include <libtimeit/db/AutotrackAccessor.h>
#include <libtimeit/db/ExtendedTaskAccessor.h>
#include <libtimeit/db/TaskAccessor.h>
#include <libtimeit/db/SettingsAccessor.h>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/Database.h>

namespace libtimeit
{
using namespace std;

void Database::createTablesPartOne()
{
	db.exe("PRAGMA foreign_keys = ON");
	//Create necessary tables
	TimeAccessor time_accessor(*this);
	TaskAccessor task_accessor(*this);
	time_accessor.createTable();
	task_accessor.createTable();
	db.exe("CREATE TABLE IF NOT EXISTS autotrack (taskID INTEGER, workspace   INTEGER)");
	db.exe("CREATE TABLE IF NOT EXISTS parameters (id VARCHAR PRIMARY KEY, string      VARCHAR, value       INTEGER, boolean     BOOL) ");
	db.exe("CREATE TABLE IF NOT EXISTS settings(name  VARCHAR PRIMARY KEY, intValue    INTEGER, boolValue   BOOL, stringValue VARCHAR) ");
}



int Database::getCurrentDBVersion()
{
	int DBVersion = 0;
	stringstream statement;
	statement << "SELECT value FROM parameters ";
	statement << " WHERE id == \"dbversion\"";
	//Do upgrade if necessary
	QueryResult rows = db.exe(statement.str());
	if (rows.size() > 0)
	{
		vector<DataCell> row = rows.at(0);
		DBVersion = row[0].getInt();
	}
	return DBVersion;
}

void Database::upgradeIfNeeded(int DBVersion, const int expectedDBVersion)
{
	if (DBVersion != expectedDBVersion)
	{
		stringstream statement;
		if (DBVersion == 0)
		{
			//Empty database. Populate with data
			statement.str("INSERT INTO parameters (id, value) VALUES ( \"dbversion\", 1 )");
			db.exe(statement.str());
		}
		else
		{
			//Upgrade existing
			//LCOV_EXCL_START
			if (DBVersion < 4)
			{
				db.exe("DROP VIEW v_timesSummary;");
				db.exe("DROP VIEW v_tasks;");
			}
			//LCOV_EXCL_STOP
			if (DBVersion < 5)
			{
				TaskAccessor task_accessor(*this);
				TimeAccessor time_accessor(*this);
				task_accessor.upgradeToDB5();
				time_accessor.upgradeToDB5();
			}
		}
		statement.str("");
		statement << "UPDATE parameters SET value = ";
		statement << expectedDBVersion;
		statement << " WHERE id = \"dbversion\";";
		db.exe(statement.str());
	}
}

void Database::createViews()
{
	//Create views
	db.exe("DROP VIEW IF EXISTS v_running");
	db.exe("CREATE VIEW v_running AS SELECT taskID, running FROM times WHERE running =\"1\"");
	db.exe("DROP VIEW IF EXISTS v_tasks");
	db.exe(
			"CREATE VIEW v_tasks AS SELECT tasks.*, IFNULL(v_running.running,0) as running FROM tasks    LEFT JOIN v_running    ON tasks.id=v_running.taskId");
	TimeAccessor time_accessor(*this);
	time_accessor.createViews();
}

Database::Database(
		const std::string& dbname, Notifier& notifier)
		:
		m_notifier(notifier),
		db(dbname)
{
	try
	{
		db.exe("PRAGMA foreign_keys = OFF");
		beginTransaction();

		createTablesPartOne();

		const int expectedDBVersion = 5;

		int DBVersion = getCurrentDBVersion();
		upgradeIfNeeded(DBVersion, expectedDBVersion);
		createViews();

		endTransaction();
		db.exe("PRAGMA foreign_keys = ON");

		TimeAccessor time_accessor(*this);
		time_accessor.removeShortTimeSpans();
	//LCOV_EXCL_START
	} catch (dbexception& e)
	{
		tryRollback();
		cerr << "Initiating DB with " << dbname << " caused: " << e.what() << endl;
		throw;
	}
	//LCOV_EXCL_STOP

}

Database::~Database()
{
}

bool Database::isThreadSafe()
{
	return db.isThreadSafe();
}


void Database::beginTransaction()
{
	db.beginTransaction();
}

void Database::tryRollback()
{
	db.tryRollback();
}

void Database::endTransaction()
{
	db.endTransaction();
}
void Database::enableNotifications(bool state)
{
	m_notifier.enabled(state);
}

void Database::sendNotification(MessageType type, int64_t taskId)
{
	m_notifier.sendNotification(type,taskId);
}

void Database::attach(EventObserver *observer)
{
	m_notifier.attach(observer);
}

void Database::detach(EventObserver *observer)
{
	m_notifier.detach(observer);
}

QueryResult Database::exe(string statement)
{
	return db.exe(statement);
}
Statement Database::prepare(string statement)
{
	return db.prepare(statement.c_str());
}

int64_t Database::getIDOfLastInsert()
{
	return db.getIDOfLastInsert();
}

}
