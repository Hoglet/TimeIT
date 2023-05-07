#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/extended_task_accessor.h>

namespace test
{
using namespace libtimeit;

extended_task get_task()
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	extended_task_accessor tasks( tempdb);
	time_accessor times( tempdb);
	task test_task( "test" );
	auto task_id = test_task.id;
	tasks.create( test_task);
	task sub_task( "sub_task", task_id);
	auto sub_task_id = sub_task.id;
	tasks.create( sub_task );
	auto duration1 = system_clock::from_time_t( 200 ) - system_clock::from_time_t( 100 );
	auto duration2 = system_clock::from_time_t( 200 ) - system_clock::from_time_t( 150 );
	times.create( time_entry( task_id, system_clock::from_time_t( 100 ), duration_cast<seconds>(duration1)) );
	times.create( time_entry( sub_task_id, system_clock::from_time_t( 150 ), duration_cast<seconds>(duration2)) );
	return *tasks.by_id( task_id);
}

TEST(TimeManagement, tasksTimeIs100)
{
	extended_task task_1 = get_task( );
	ASSERT_EQ( 100s, task_1.time);
}

TEST(TimeManagement, tasksTotalTimeIs150)
{
	extended_task task_1 = get_task( );
	ASSERT_EQ( 150s, task_1.total_time);
}


}
