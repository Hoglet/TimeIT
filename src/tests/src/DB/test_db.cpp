#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/database.h>
#include <libtimeit/os_abstraction.h>
#include <libtimeit/exceptions/db_exception.h>
#include <libtimeit/db/data_cell.h>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <sstream>
#include <string>

using namespace libtimeit;
using namespace std;

namespace test
{

void createVersion4db(const char *dbname)
{
	unlink(dbname);
	SQLite3 db(dbname);

	//Create necessary tables
	db.execute(
			"CREATE TABLE IF NOT EXISTS tasks "
			"(id          INTEGER PRIMARY KEY,"
			" name        VARCHAR,"
			" parent      INTEGER,"
			" running     BOOL DEFAULT false,"
			" expanded    BOOL DEFAULT false,"
			" deleted     BOOL DEFAULT false)");
	db.execute(
			"CREATE TABLE IF NOT EXISTS times "
			"(id          INTEGER PRIMARY KEY,"
			" taskID      INTEGER,"
			" start       INTEGER,"
			" stop        INTEGER,"
			" running     BOOL    DEFAULT false) ");
	db.execute(
			"CREATE TABLE IF NOT EXISTS autotrack "
			"(taskID      INTEGER,"
			" workspace   INTEGER)");
	db.execute(
			"CREATE TABLE IF NOT EXISTS parameters "
			"(id          VARCHAR PRIMARY KEY,"
			" string      VARCHAR,"
			" value       INTEGER,"
			" boolean     BOOL) ");
	db.execute(
			"CREATE TABLE IF NOT EXISTS settings"
			"(name        VARCHAR PRIMARY KEY,"
			" intValue    INTEGER,"
			" boolValue   BOOL,"
			" stringValue VARCHAR) ");

	const int expectedDBVersion = 4;
	stringstream statement;
	statement << "INSERT INTO parameters (id, value) VALUES ( \"dbversion\"," << expectedDBVersion << " )";
	db.execute(statement.str());
	db.execute("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (1,'Test',0,0,1);");
	db.execute("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (2,'Sub task',1,1,0);");
	db.execute("INSERT INTO times (id,taskID,start,stop,running) VALUES (1,2,10,100,0);");
}

void openAndCloseDB()
{
	notification_manager notifier;
	database db("/tmp/dbtest.db", notifier);
}

TEST( database, testCreation )
{
	//Testing creation
	openAndCloseDB();
	openAndCloseDB();

	unlink("/tmp/dbtest.db");

	//Testing failing creation
	notification_manager notifier;
	ASSERT_THROW(database db("not/existing/path/dbtest.db", notifier), db_exception);
}

TEST( database, testUpgrade )
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	notification_manager notifier;
	database db("/tmp/dbtest.db", notifier);
	task_accessor taskAccessor(db);
	ASSERT_EQ( 2, taskAccessor.changed_since().size()) << "Numbers of tasks in tasks";
	auto task1 = taskAccessor.by_id(1);
	ASSERT_EQ( string("Test"), task1->name) << "Task 1 name ";
	ASSERT_EQ( 0, task1->parent_id) << "Task 1 parent ";

	auto task2 = taskAccessor.by_id(2);
	ASSERT_EQ( string("Sub task"), task2->name) << "Task 2 name ";
	ASSERT_EQ( 1, task2->parent_id) << "Task 2 parent ";

	time_accessor timeAccessor(db);
	vector<Time_entry> times = timeAccessor.times_changed_since();
	ASSERT_EQ( 1, times.size()) << "Number of times ";
	Time_entry te = times.at(0);
	ASSERT_EQ( 1, te.id) << "Time id ";
	ASSERT_EQ( 2, te.task_id) << "Time taskID ";
	ASSERT_EQ( 10, te.start) << "Time start ";
	ASSERT_EQ( 100, te.stop) << "Time stop ";

	unlink("/tmp/dbtest.db");
}

TEST( database, testDatacell )
{
	std::string data("Text");
	data_cell dc1(data.c_str());
	data_cell dc2(dc1);

	ASSERT_EQ( data, dc2.text());
	ASSERT_THROW((void) dc2.integer(), db_exception);
	ASSERT_THROW((void) dc2.boolean(), db_exception);

	data_cell dci(1);
	ASSERT_EQ(1, dci.integer());
	ASSERT_THROW(dci.text(), db_exception);

}

TEST( database, testSQLErrorManagement)
{
	const char *dbname = "/tmp/dbtest.db";
	createVersion4db(dbname);
	SQLite3 db(dbname);
	string faultyStatement = "select * from tmies";
	ASSERT_THROW(db.execute(faultyStatement), db_exception);
}

TEST( database, testTransactions)
{
	notification_manager notifier;
	TempDB db(notifier);
	db.begin_transaction();
	task_accessor taskAccessor(db);
	taskAccessor.create(task("Test", 0));
	ASSERT_EQ( 1, taskAccessor.changed_since().size()) << "Checking number of tasks after adding one";
	db.end_transaction();
	ASSERT_EQ( 1, taskAccessor.changed_since().size()) << "Checking number of tasks after commit";
}

TEST( database, testTransactions_rollback )
{
	notification_manager notifier;
	TempDB db(notifier);
	db.begin_transaction();
	task_accessor taskAccessor(db);
	taskAccessor.create(task("Test", 0));
	ASSERT_EQ( 1, taskAccessor.changed_since().size()) << "Checking number of tasks after adding one";
	db.try_rollback();
	ASSERT_EQ( 0, taskAccessor.changed_since().size()) << "Checking number of tasks after rollback";
}

TEST( database, testTransactionsSanity)
{
	notification_manager notifier;
	TempDB db(notifier);
	db.begin_transaction();
	ASSERT_THROW(db.begin_transaction(), db_exception);
	ASSERT_THROW(db.end_transaction(), db_exception);
}

} /* namespace Test */
