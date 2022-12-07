#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <sstream>
#include <string>

using namespace libtimeit;
using namespace std;

namespace test
{

void create_version_4_db( const char *dbname)
{
	unlink(dbname);
	sqlite db( dbname);

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

	const int expected_db_version = 4;
	stringstream statement;
	statement << "INSERT INTO parameters (id, value) VALUES ( \"dbversion\"," << expected_db_version << " )";
	db.execute(statement.str());
	db.execute("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (1,'Test',0,0,1);");
	db.execute("INSERT INTO tasks (id,name,parent,running,expanded) VALUES (2,'Sub task',1,1,0);");
	db.execute("INSERT INTO times (id,taskID,start,stop,running) VALUES (1,2,10,100,0);");
}

void open_and_close_db()
{
	notification_manager notifier;
	database db("/tmp/dbtest.db", notifier);
}

TEST( database, testCreation )
{
	//Testing creation
	unlink("/tmp/dbtest.db");
	open_and_close_db( );
	open_and_close_db( );

	unlink("/tmp/dbtest.db");

	//Testing failing creation
	notification_manager notifier;
	ASSERT_THROW(database db("not/existing/path/dbtest.db", notifier), db_exception);
}

TEST( database, testUpgrade )
{
	const char *dbname = "/tmp/dbtest.db";
	create_version_4_db( dbname );
	notification_manager notifier;
	database db("/tmp/dbtest.db", notifier);
	task_accessor tasks( db);
	ASSERT_EQ( 2, tasks.changed_since().size()) << "Numbers of tasks in tasks";

/*
    auto task1 = taskAccessor.by_id(1);
	ASSERT_EQ( string("Test"), task1->name) << "Task 1 name ";
	ASSERT_EQ( 0, task1->parent_id) << "Task 1 parent ";

	auto task2 = taskAccessor.by_id(2);
	ASSERT_EQ( string("Sub task"), task2->name) << "Task 2 name ";
	ASSERT_EQ( 1, task2->parent_id) << "Task 2 parent ";
*/
	time_accessor times( db);
	vector<time_entry> result = times.times_changed_since();
	ASSERT_EQ( 1, result.size()) << "Number of times ";
	time_entry te = result.at( 0);
	ASSERT_EQ( system_clock::from_time_t( 10 ), te.start) << "Time start ";
	ASSERT_EQ( system_clock::from_time_t( 100 ), te.stop) << "Time stop ";

	unlink("/tmp/dbtest.db");
}

TEST( database, testDatacell )
{
	std::string data("Text");
	data_cell dc_1( data.c_str());
	data_cell dc_2( dc_1);

	ASSERT_EQ( data, dc_2.text());
	ASSERT_THROW((void) dc_2.integer(), db_exception);
	ASSERT_THROW((void) dc_2.boolean(), db_exception);

	data_cell dci(1);
	ASSERT_EQ(1, dci.integer());
	ASSERT_THROW(dci.text(), db_exception);

}

TEST( database, testSQLErrorManagement)
{
	const char *dbname = "/tmp/dbtest.db";
	create_version_4_db( dbname );
	sqlite db( dbname);
	string faulty_statement = "select * from tmies";
	ASSERT_THROW( db.execute( faulty_statement), db_exception);
}

TEST( database, testTransactions)
{
	notification_manager notifier;
	temp_db db(notifier);
	db.begin_transaction();
	task_accessor tasks( db);
	tasks.create( task( "Test"));
	ASSERT_EQ( 1, tasks.changed_since().size()) << "Checking number of tasks after adding one";
	db.end_transaction();
	ASSERT_EQ( 1, tasks.changed_since().size()) << "Checking number of tasks after commit";
}

TEST( database, testTransactions_rollback )
{
	notification_manager notifier;
	temp_db db(notifier);
	db.begin_transaction();
	task_accessor tasks( db);
	tasks.create( task( "Test"));
	ASSERT_EQ( 1, tasks.changed_since().size()) << "Checking number of tasks after adding one";
	db.try_rollback();
	ASSERT_EQ( 0, tasks.changed_since().size()) << "Checking number of tasks after rollback";
}

TEST( database, testTransactionsSanity)
{
	notification_manager notifier;
	temp_db db(notifier);
	db.begin_transaction();
	ASSERT_THROW(db.begin_transaction(), db_exception);
	ASSERT_THROW(db.end_transaction(), db_exception);
}

} /* namespace test */
