#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/extended_task_accessor.h>

namespace test
{


using namespace libtimeit;
using namespace std;

TEST(ExtendedTaskAccessor, by_ID)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	extended_task_accessor tasks( tempdb);

	task parent( "Test", {});
	task test_task_2( "Test2", parent.id);
	tasks.create( parent);
	tasks.create( test_task_2);

	time_accessor times( tempdb);
	times.create( time_entry( parent.id, system_clock::from_time_t( 0 ),  1000s ) );
	times.create( time_entry( test_task_2.id, system_clock::from_time_t( 0 ), 1000s ) );

	auto task_1 = tasks.by_id( parent.id);
	ASSERT_EQ( "Test", task_1->name);
	ASSERT_EQ( 2000s, task_1->total_time);

	auto task_2 = tasks.by_id( test_task_2.id);
	ASSERT_EQ( "Test2", task_2->name);
	ASSERT_EQ( 1000s, task_2->total_time);
}

TEST(ExtendedTaskAccessor, by_parent_ID)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	extended_task_accessor tasks( tempdb);
	time_accessor times( tempdb);

	task test_task( "Test", {} );
	tasks.create( test_task );

	vector<extended_task> result = tasks.by_parent_id( {});
	ASSERT_EQ( 1, result.size());

	task next_task("NextTask", {});
	tasks.create( next_task);
	result = tasks.by_parent_id( {});
	ASSERT_EQ( 2, result.size());

	task test_task_2("Test2", test_task.id);
	tasks.create( test_task_2);
	times.create( time_entry( test_task_2.id, system_clock::from_time_t( 0 ), 1000s ) );

	result = tasks.by_parent_id( {});
	extended_task &task_1 = result.at( 0);
	ASSERT_EQ( "Test", task_1.name);
	ASSERT_EQ( 1000s, task_1.total_time);
}

TEST(ExtendedTaskAccessor, testTotalTime)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	extended_task_accessor tasks( tempdb );
	task test_task_1("Test", {});
	task test_task_2("Test2", test_task_1.id);
	tasks.create( test_task_1 );
	tasks.create( test_task_2);

	time_accessor times( tempdb);
	times.create( time_entry( test_task_2.id, system_clock::from_time_t( 0 ), 1000s ));

	auto task_1 = tasks.by_id( test_task_1.id);
	ASSERT_EQ( 1000s, task_1->total_time);
}

TEST(ExtendedTaskAccessor, setExpandedTasks)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	extended_task_accessor tasks( tempdb);

	task test_task("Test", {});
	tasks.create( test_task);
	auto task_id = test_task.id;
	auto task_1 = tasks.by_id( task_id);
	ASSERT_EQ( false, task_1->expanded);
	tasks.set_task_expanded( task_id, true);
	auto task_2 = tasks.by_id( task_id);
	ASSERT_EQ( true, task_2->expanded);
}

TEST(ExtendedTaskAccessor, testTimeReporting)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	extended_task_accessor tasks( tempdb);
	task parent("test", {});
	tasks.create( parent );
	auto parent_id = parent.id;
	task test_task( "test", parent_id);
	tasks.create( test_task);
	auto task_id = test_task.id;
	times.create( time_entry( task_id, system_clock::from_time_t( 4000 ), 1000s ) );

	auto result_1 = tasks.by_parent_id( parent_id);
	extended_task task_1 = result_1.at( 0);
	auto result_2 = task_1.total_time;
	ASSERT_EQ( 1000s, result_2);

	auto task_2 = tasks.by_id( task_id);
	result_2 = task_2->total_time;
	ASSERT_EQ( 1000s, result_2);

	result_1 = tasks.by_parent_id( {});
	auto task_3 = result_1.at( 0);
	result_2 = task_1.total_time;
	ASSERT_EQ( 1000s, result_2);

	result_1 = tasks.by_parent_id( parent_id);
	auto task_4 = result_1.at( 0);
	result_2 = task_4.total_time;
	ASSERT_EQ( 1000s, result_2);

}

}
