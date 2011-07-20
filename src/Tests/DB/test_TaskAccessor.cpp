#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TaskAccessor.h"
#include "Database.h"


void thisIsAtest_TaskAccessorTest() {
	//Create a database object
	boost::shared_ptr<DB::Database> database = boost::shared_ptr<DB::Database>(new DB::Database("/tmp/test.db"));


	boost::shared_ptr<ITaskAccessor> ta = database->getTaskAccessor();

	ta->newTask("test", 0);

	ASSERTM("start writing tests", false);
}

cute::suite make_suite_test_TaskAccessor(){
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_TaskAccessorTest));
	return s;
}



