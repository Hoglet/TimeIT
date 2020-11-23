#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/Database.h>
#include <libtimeit/OSAbstraction.h>
#include <libtimeit/exceptions/dbexception.h>
#include <libtimeit/db/DataCell.h>
#include <libtimeit/db/CSQL.h>
#include <sstream>
#include <string>

using namespace libtimeit;
using namespace std;

namespace test
{

void createVersion4db(const char *dbname)
{
	shared_ptr<CSQL> db = shared_ptr<CSQL>(new CSQL(dbname));

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
	stringstream statement;
	statement << "INSERT INTO parameters (id, value) VALUES ( \"dbversion\"," << expectedDBVersion << " )";
	db->exe(statement.str());
	db->exe("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (1,'Test',0,0,1);");
	db->exe("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (2,'Sub task',1,1,0);");
	db->exe("INSERT INTO times (id,taskID,start,stop,running) VALUES (1,2,10,100,0);");
}

void openAndCloseDB()
{
	Notifier notifier;
	Database db("/tmp/dbtest.db", notifier);
}

TEST( database, testCreation )
{
	//Testing creation
	openAndCloseDB();
	openAndCloseDB();

	OSAbstraction::unlink("/tmp/dbtest.db");

	//Testing failing creation
	Notifier notifier;
	ASSERT_THROW(Database db("not/existing/path/dbtest.db", notifier), dbexception);
}

TEST( database, testUpgrade )
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	Notifier notifier;
	Database db("/tmp/dbtest.db", notifier);
	shared_ptr<ITaskAccessor> taskAccessor = db.getTaskAccessor();
	ASSERT_EQ( 2, taskAccessor->getTasksChangedSince()->size()) << "Numbers of tasks in tasks";
	shared_ptr<Task> task1 = taskAccessor->getTask(1);
	ASSERT_EQ( string("Test"), task1->name()) << "Task 1 name ";
	ASSERT_EQ( 0, task1->parentID()) << "Task 1 parent ";

	shared_ptr<Task> task2 = taskAccessor->getTask(2);
	ASSERT_EQ( string("Sub task"), task2->name()) << "Task 2 name ";
	ASSERT_EQ( 1, task2->parentID()) << "Task 2 parent ";

	shared_ptr<ITimeAccessor> timeAccessor = db.getTimeAccessor();
	vector<TimeEntry> times = timeAccessor->getTimesChangedSince();
	ASSERT_EQ( 1, times.size()) << "Number of times ";
	TimeEntry te = times.at(0);
	ASSERT_EQ( 1, te.ID()) << "Time id ";
	ASSERT_EQ( 2, te.taskID()) << "Time taskID ";
	ASSERT_EQ( 10, te.start()) << "Time start ";
	ASSERT_EQ( 100, te.stop()) << "Time stop ";

	OSAbstraction::unlink("/tmp/dbtest.db");
}

TEST( database, testDatacell )
{
	std::string data("Text");
	DataCell dc1(data.c_str());
	DataCell dc2("");

	dc2 = dc1;
	ASSERT_EQ( data, dc2.getString());
	ASSERT_THROW(dc2.getInt(), dbexception);
	ASSERT_THROW(dc2.getBool(), dbexception);

	DataCell dci(1);
	ASSERT_EQ(1, dci.getInt());
	ASSERT_THROW(dci.getString(), dbexception);

}

TEST( database, testSQLErrorManagement)
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	CSQL db(dbname);
	string faultyStatement = "select * from tmies";
	ASSERT_THROW(db.exe(faultyStatement), dbexception);
}

TEST( database, testTransactions)
{
	Notifier notifier;
	TempDB db(notifier);
	db.beginTransaction();
	std::shared_ptr<ITaskAccessor> taskAccessor = db.getTaskAccessor();
	taskAccessor->newTask("Test", 0);
	ASSERT_EQ( 1, taskAccessor->getTasksChangedSince()->size()) << "Checking number of tasks after adding one";
	db.endTransaction();
	ASSERT_EQ( 1, taskAccessor->getTasksChangedSince()->size()) << "Checking number of tasks after commit";
}

TEST( database, testTransactions_rollback )
{
	Notifier notifier;
	TempDB db(notifier);
	db.beginTransaction();
	std::shared_ptr<ITaskAccessor> taskAccessor = db.getTaskAccessor();
	taskAccessor->newTask("Test", 0);
	ASSERT_EQ( 1, taskAccessor->getTasksChangedSince()->size()) << "Checking number of tasks after adding one";
	db.tryRollback();
	ASSERT_EQ( 0, taskAccessor->getTasksChangedSince()->size()) << "Checking number of tasks after rollback";
}

TEST( database, testTransactionsSanity)
{
	Notifier notifier;
	TempDB db(notifier);
	db.beginTransaction();
	ASSERT_THROW(db.beginTransaction(), dbexception);
	ASSERT_THROW(db.endTransaction(), dbexception);
}

} /* namespace Test */
