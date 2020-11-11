#include <UUIDTool.h>
#include "test_db.h"
#include "cute.h"
#include "Database.h"
#include "OSAbstraction.h"
#include "dbexception.h"
#include "DataCell.h"
#include "CSQL.h"
#include "TempDB.h"
#include <sstream>
#include <string>

using namespace DBAbstraction;
using namespace DB;
using namespace std;

namespace Test
{

void createVersion4db(const char *dbname)
{
	std::shared_ptr<CSQL> db = std::shared_ptr<CSQL>(new CSQL(dbname));

	//Create necessary tables
	db->exe("CREATE TABLE IF NOT EXISTS tasks "
			"(id          INTEGER PRIMARY KEY,"
			" name        VARCHAR,"
			" parent      INTEGER,"
			" running     BOOL DEFAULT false,"
			" expanded    BOOL DEFAULT false,"
			" deleted     BOOL DEFAULT false)");
	db->exe("CREATE TABLE IF NOT EXISTS times "
			"(id          INTEGER PRIMARY KEY,"
			" taskID      INTEGER,"
			" start       INTEGER,"
			" stop        INTEGER,"
			" running     BOOL    DEFAULT false) ");
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

	const int expectedDBVersion = 4;
	std::stringstream statement;
	statement << "INSERT INTO parameters (id, value) VALUES ( \"dbversion\"," << expectedDBVersion << " )";
	db->exe(statement.str());
	db->exe("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (1,'Test',0,0,1);");
	db->exe("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (2,'Sub task',1,1,0);");
	db->exe("INSERT INTO times (id,taskID,start,stop,running) VALUES (1,2,10,100,0);");
}

void openAndCloseDB()
{
	DB::Database db("/tmp/dbtest.db");
}

void database_testCreation()
{
	//Testing creation
	openAndCloseDB();
	openAndCloseDB();

	OSAbstraction::unlink("/tmp/dbtest.db");

	//Testing failing creation
	ASSERT_THROWS(DB::Database db("not/existing/path/dbtest.db"), dbexception);
}

void database_testUpgrade()
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	Database db("/tmp/dbtest.db");
	shared_ptr<ITaskAccessor> taskAccessor = db.getTaskAccessor();
	ASSERT_EQUALM("Numbers of tasks in tasks", 2, taskAccessor->getTasksChangedSince()->size());
	shared_ptr<Task> task1 = taskAccessor->getTask(1);
	ASSERT_EQUALM("Task 1 name ", string("Test"), task1->name());
	ASSERT_EQUALM("Task 1 parent ", 0, task1->parentID());

	shared_ptr<Task> task2 = taskAccessor->getTask(2);
	ASSERT_EQUALM("Task 2 name ", string("Sub task"), task2->name());
	ASSERT_EQUALM("Task 2 parent ", 1, task2->parentID());

	shared_ptr<ITimeAccessor> timeAccessor = db.getTimeAccessor();
	vector<TimeEntry> times = timeAccessor->getTimesChangedSince();
	ASSERT_EQUALM("Number of times ", 1, times.size());
	TimeEntry te = times.at(0);
	ASSERT_EQUALM("Time id ", 1, te.ID());
	ASSERT_EQUALM("Time taskID ", 2, te.taskID());
	ASSERT_EQUALM("Time start ", 10, te.start());
	ASSERT_EQUALM("Time stop ", 100, te.stop());

	OSAbstraction::unlink("/tmp/dbtest.db");
}

void database_testDatacell()
{
	std::string data("Text");
	DBAbstraction::DataCell dc1(data.c_str());
	DBAbstraction::DataCell dc2("");

	dc2 = dc1;
	ASSERT_EQUAL(data, dc2.getString());
	ASSERT_THROWS(dc2.getInt(), dbexception);
	ASSERT_THROWS(dc2.getBool(), dbexception);

	DBAbstraction::DataCell dci(1);
	ASSERT_EQUAL(1, dci.getInt());
	ASSERT_THROWS(dci.getString(), dbexception);

}

void database_testSQLErrorManagement()
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	CSQL db(dbname);
	std::string faultyStatement = "select * from tmies";
	ASSERT_THROWS(db.exe(faultyStatement), dbexception);
}

void database_testTransactions()
{
	TempDB db;
	db.beginTransaction();
	std::shared_ptr<DB::ITaskAccessor> taskAccessor = db.getTaskAccessor();
	taskAccessor->newTask("Test", 0);
	ASSERT_EQUALM("Checking number of tasks after adding one", 1, taskAccessor->getTasksChangedSince()->size());
	db.endTransaction();
	ASSERT_EQUALM("Checking number of tasks after commit", 1, taskAccessor->getTasksChangedSince()->size());
}

void database_testTransactions_rollback()
{
	TempDB db;
	db.beginTransaction();
	std::shared_ptr<DB::ITaskAccessor> taskAccessor = db.getTaskAccessor();
	taskAccessor->newTask("Test", 0);
	ASSERT_EQUALM("Checking number of tasks after adding one", 1, taskAccessor->getTasksChangedSince()->size());
	db.tryRollback();
	ASSERT_EQUALM("Checking number of tasks after rollback", 0, taskAccessor->getTasksChangedSince()->size());
}

void database_testTransactionsSanity()
{
	TempDB db;
	db.beginTransaction();
	ASSERT_THROWS(db.beginTransaction(), dbexception);
	ASSERT_THROWS(db.endTransaction(), dbexception);
}

cute::suite make_suite_DB_test()
{
	cute::suite s;
	s.push_back(CUTE(database_testCreation));
	s.push_back(CUTE(database_testUpgrade));
	s.push_back(CUTE(database_testDatacell));
	s.push_back(CUTE(database_testSQLErrorManagement));
	s.push_back(CUTE(database_testTransactions));
	s.push_back(CUTE(database_testTransactions_rollback));
	s.push_back(CUTE(database_testTransactionsSanity));
	return s;
}
} /* namespace Test */
