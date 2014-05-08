#include "TimeAccessor.h"
#include "internal/AutotrackAccessor.h"
#include "ExtendedTaskAccessor.h"
#include "TaskAccessor.h"
#include "internal/SettingsAccessor.h"
#include "CSQL.h"
#include "Database.h"

#include <iostream>
#include <sstream>

namespace DB
{
using namespace std;
using namespace DBAbstraction;

Database::Database(const std::string& dbname)
{
	UUID::init();

	db = shared_ptr<CSQL>(new CSQL(dbname));

	db->exe("PRAGMA foreign_keys = ON");
	//Create necessary tables
	db->exe("CREATE TABLE IF NOT EXISTS autotrack "
			"(taskID      INTEGER,"
			" workspace   INTEGER)");
	db->exe("CREATE TABLE IF NOT EXISTS parameters "
			"(id          VARCHAR PRIMARY KEY,"
			" string      VARCHAR,"
			" value       INTEGER,"
			" boolean     BOOL) ");
	db->exe("CREATE TABLE IF NOT EXISTS settings"
			"(name        VARCHAR PRIMARY KEY,"
			" intValue    INTEGER,"
			" boolValue   BOOL,"
			" stringValue VARCHAR) ");

	try
	{
		//Need to disable foreign keys in case of upgrade of task table.
		db->exe("PRAGMA foreign_keys = OFF");
		beginTransaction();
		std::shared_ptr<Notifier> notifier(new Notifier());
		m_notifier = notifier;
		m_timeAccessor = std::shared_ptr<TimeAccessor>(new TimeAccessor(db, notifier));
		m_timeAccessor->createTable();
		m_taskAccessor = std::shared_ptr<TaskAccessor>(new TaskAccessor(db, notifier));
		m_taskAccessor->createTable();
		m_extendedTaskAccessor = std::shared_ptr<ExtendedTaskAccessor>(
				new ExtendedTaskAccessor(db, notifier, m_timeAccessor));
		m_autotrackAccessor = std::shared_ptr<AutotrackAccessor>(new AutotrackAccessor(db, m_extendedTaskAccessor));
		m_settingsAccessor = std::shared_ptr<SettingsAccessor>(new SettingsAccessor(db));

		stringstream statement;
		statement << "SELECT value FROM parameters ";
		statement << " WHERE id == \"dbversion\"";

		//Do upgrade if necessary
		int DBVersion = 0;
		const int expectedDBVersion = 5;

		std::shared_ptr<QueryResult> rows = db->exe(statement.str());
		if (rows->size() > 0)
		{
			vector<DataCell> row = rows->at(0);
			DBVersion = row[0].getInt();
		}
		if (DBVersion != expectedDBVersion)
		{
			if (DBVersion == 0)
			{ //Empty database. Populate with data
				statement.str("INSERT INTO parameters (id, value) VALUES ( \"dbversion\", 1 )");
				db->exe(statement.str());
			} else
			{ //Upgrade existing
			  //LCOV_EXCL_START
				if (DBVersion < 4)
				{
					db->exe("DROP VIEW v_timesSummary;");
					db->exe("DROP VIEW v_tasks;");
				}
				//LCOV_EXCL_STOP

				if (DBVersion < 5)
				{
					m_taskAccessor->upgradeToDB5();
					m_timeAccessor->upgradeToDB5();
				}
			}

			statement.str("");
			statement << "UPDATE parameters SET value = ";
			statement << expectedDBVersion;
			statement << " WHERE id = \"dbversion\";";
			db->exe(statement.str());
		}

		//Create views
		db->exe("DROP VIEW IF EXISTS v_running");
		db->exe("CREATE VIEW v_running AS SELECT taskID, running FROM times WHERE running =\"1\"");

		db->exe("DROP VIEW IF EXISTS v_tasks");
		db->exe("CREATE VIEW v_tasks AS"
				" SELECT tasks.*, IFNULL(v_running.running,0) as running"
				" FROM tasks "
				"   LEFT JOIN v_running "
				"   ON tasks.id=v_running.taskId");
		m_timeAccessor->createViews();

		endTransaction();
		db->exe("PRAGMA foreign_keys = ON");

		//LCOV_EXCL_START
	} catch (dbexception& e)
	{
		tryRollback();
		cerr << "Initiating DB with " << dbname << " caused: " << e.what() << endl;
		throw e;
	}
	//LCOV_EXCL_STOP

}

Database::~Database()
{
}

bool Database::isThreadSafe()
{
	return db->isThreadSafe();
}


void Database::beginTransaction()
{
	db->beginTransaction();
}

void Database::tryRollback()
{
	db->tryRollback();
}

void Database::endTransaction()
{
	db->endTransaction();
}

std::shared_ptr<IAutotrackAccessor> Database::getAutotrackAccessor()
{
	return m_autotrackAccessor;
}

std::shared_ptr<ITimeAccessor> Database::getTimeAccessor()
{
	return m_timeAccessor;
}

std::shared_ptr<IExtendedTaskAccessor> Database::getExtendedTaskAccessor()
{
	return m_extendedTaskAccessor;
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
