#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/extended_task_accessor.h>
#include "notify_observer.h"

namespace test
{


using namespace libtimeit;
using namespace std;

const string COMMENT = "Just a comment";

TEST(TimeAccessor, simpleTest)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	task_accessor tasks( tempdb);
	task test_task( "test" );
	tasks.create( test_task);
	times.create( time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 )) );
	auto result = times.duration_time( test_task.id);
	ASSERT_EQ(1000, result.count());
}

TEST(TimeAccessor, ChangeEndTime)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task test_task( "test" );
	tasks.create( test_task );
	time_accessor times( tempdb);
	time_entry original = time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	times.create( original );
	auto te = times.by_id( original.id );
	if (te)
	{
		times.update( te->with_stop( system_clock::from_time_t( 1300 )));
	}
	auto result = times.duration_time( test_task.id);
	ASSERT_EQ(1300, result.count());

}

TEST(TimeAccessor, ChangeStartTime)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task test_task( "test" );
	tasks.create( test_task );
	time_accessor times( tempdb);
	time_entry original( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	times.create( original );
	auto te = times.by_id( original.id);
	times.update( te->with_start( system_clock::from_time_t( 300 )));
	auto result = times.duration_time( test_task.id);
	ASSERT_EQ(700, result.count());

}

TEST(TimeAccessor, UpdateTime)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	notify_observer observer( notifier);

	task_accessor tasks( tempdb);
	task test_task( "test" );
	tasks.create( test_task );
	time_accessor times( tempdb);
	time_entry first( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	times.create( first );
	observer.task_id_time = {};
	auto original = times.by_id( first.id).value();
	times.update( original.with_start( system_clock::from_time_t( 300 )).with_stop( system_clock::from_time_t( 700 )));
	auto result = times.duration_time( test_task.id);
	ASSERT_EQ(400s, result );
	ASSERT_TRUE( observer.task_id_time.has_value());
	ASSERT_EQ(observer.task_id_time.value(), test_task.id );

}

TEST(TimeAccessor, RemoveTime)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task test_task("test");
	tasks.create( test_task );
	time_accessor times( tempdb);
	time_entry te = time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	times.create( te );
	times.create( time_entry( test_task.id, system_clock::from_time_t( 2000 ), system_clock::from_time_t( 2300 )));
	times.remove( te );
	auto result = times.duration_time( test_task.id );
	ASSERT_EQ(300s, result);
}

TEST(TimeAccessor, GetLatestTasks)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);

	task test_task( "test" );
	tasks.create( test_task );
	time_accessor times( tempdb);
	times.create( time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000)));
	auto result = times.latest_active_tasks( 10);
	ASSERT_EQ(1, result.size());
	ASSERT_EQ( test_task.id, result[0]);
	tasks.remove( test_task.id);
	result = times.latest_active_tasks( 10);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, GetDetailTimeList)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	extended_task_accessor tasks( tempdb);

	task test_task( "test" );
	tasks.create( test_task );
	times.create( time_entry( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 )) );
	vector<time_entry> result = times.by_activity(
			test_task.id,
			system_clock::from_time_t( 0 ),
			system_clock::from_time_t( 10000 ));

	ASSERT_EQ(1, result.size());
	time_entry te = result[0];
	ASSERT_EQ(system_clock::from_time_t( 10 ), te.start);
	ASSERT_EQ(system_clock::from_time_t( 100 ), te.stop);
}

TEST(TimeAccessor, testGetByID)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	extended_task_accessor tasks( tempdb);
	task test_task( "test" );
	tasks.create( test_task );
	time_entry original( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 ));
	times.create( original );
	auto te = times.by_id( original.id);
	ASSERT_EQ( test_task.id, te->owner_id) << "Check task id";
	ASSERT_EQ(system_clock::from_time_t( 10 ), te->start) << "Check start";
	ASSERT_EQ(system_clock::from_time_t( 100 ), te->stop) << "Check stop";

}

TEST(TimeAccessor, testGetByUUID)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	extended_task_accessor tasks( tempdb);

	task test_task( "test" );
	tasks.create( test_task );
	time_entry original = time_entry( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 ));
	times.create( original );

	auto te = times.by_id( original.id);
	ASSERT_EQ( test_task.id, te->owner_id ) << "Check task id";
	ASSERT_EQ(system_clock::from_time_t( 10 ), te->start) << "Check start";
	ASSERT_EQ(system_clock::from_time_t( 100 ), te->stop) << "Check stop";
	ASSERT_EQ( original.id, te->id) << "Check id";

}




TEST(TimeAccessor, newItem)
{
	notification_manager notifier;
	temp_db db(notifier);
	time_accessor times( db);
	extended_task_accessor tasks( db);

	task test_task( "test" );
	tasks.create( test_task );
	time_entry item_1(
			time_id(),
			test_task.id,
			system_clock::from_time_t( 100 ),
			system_clock::from_time_t( 200 ),
			stopped,
			system_clock::from_time_t( 200 ),
			COMMENT);

	times.create( item_1);
	auto item_2 = times.by_id( item_1.id);

	ASSERT_EQ( item_1.id, item_2->id) << "UUID: ";
	ASSERT_EQ( item_1.owner_id, item_2->owner_id) << "Task_ID: ";
	ASSERT_EQ( item_1.start, item_2->start) << "Start: ";
	ASSERT_EQ( item_1.stop, item_2->stop) << "Stop: ";
	ASSERT_EQ( item_1.state, item_2->state) << "Deleted: ";
	ASSERT_EQ( item_1.changed, item_2->changed) << "Changed: ";
	ASSERT_EQ( item_1.comment, item_2->comment) << "Comment: ";

	ASSERT_THROW( times.create( item_1), db_exception);

}

TEST(TimeAccessor, GetTotalTimeWithChildren)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	task_accessor tasks( tempdb);

	task parent( "parent" );
	tasks.create( parent );
	auto parent_id = parent.id;
	task child( "test", parent.id );
	tasks.create( child );
	auto task_id = child.id;
	times.create( time_entry( task_id, system_clock::from_time_t( 4000 ), system_clock::from_time_t( 5000 )) );
	int parent_total_time = times.total_cumulative_time( parent_id).count();
	ASSERT_EQ( 1000, parent_total_time);
	int child_total_time = times.total_cumulative_time( task_id).count();
	ASSERT_EQ( 1000, child_total_time);
}

TEST(TimeAccessor, getTimesChangedSince)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	time_accessor times( tempdb);
	task_accessor tasks( tempdb);

	task test_task( "test" );
	auto task_id = test_task.id;
	tasks.create( test_task );

	time_entry original( task_id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	times.create( original );

	auto item = times.by_id( original.id );
	vector<time_entry> result = times.times_changed_since( system_clock::from_time_t( 0) );
	ASSERT_EQ(1, result.size());
	result = times.times_changed_since( item->changed);
	ASSERT_EQ(1, result.size());
	result = times.times_changed_since( item->changed + 1s);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, getActiveTasks)
{
	notification_manager notifier;
	temp_db db(notifier);
	time_accessor times ( db);
	extended_task_accessor tasks( db);


	task test_task( "test" );
	auto task_id = test_task.id;
	tasks.create( test_task );
	time_entry item(
			time_id(),
			task_id,
			system_clock::from_time_t( 100 ),
			system_clock::from_time_t( 600 ),
			stopped,
			system_clock::from_time_t( 200 ),
			COMMENT);

	times.create( item);

	auto result = times.active_tasks( system_clock::from_time_t( 0), system_clock::from_time_t( 50000));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = times.active_tasks( system_clock::from_time_t( 110), system_clock::from_time_t( 500));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";


	result = times.active_tasks( system_clock::from_time_t( 90), system_clock::from_time_t( 100));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = times.active_tasks( system_clock::from_time_t( 600), system_clock::from_time_t( 800));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = times.active_tasks( system_clock::from_time_t( 0), system_clock::from_time_t( 99));
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	result = times.active_tasks( system_clock::from_time_t( 601), system_clock::from_time_t( 8000));
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	tasks.remove( task_id);
	result = times.active_tasks( system_clock::from_time_t( 110), system_clock::from_time_t( 500));
	ASSERT_EQ(0, result.size()) << "Deleted task is shown";

}


}
