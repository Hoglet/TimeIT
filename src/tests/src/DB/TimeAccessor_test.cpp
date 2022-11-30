#include <gtest/gtest.h>
#include "TempDB.h"
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
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	task_accessor taskAccessor(tempdb);
	task test_task( "test" );
	taskAccessor.create(test_task);
	timeAccessor.create( time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t(  1000 )) );
	auto result = timeAccessor.duration_time( test_task.id);
	ASSERT_EQ(1000, result.count());
}

TEST(TimeAccessor, ChangeEndTime)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	task test_task( "test" );
	taskAccessor.create( test_task );
	time_accessor timeAccessor(tempdb);
	time_entry original = time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	timeAccessor.create( original );
	auto te = timeAccessor.by_id( original.id );
	if (te)
	{
		timeAccessor.update(te->with_stop( system_clock::from_time_t( 1300 )));
	}
	auto result = timeAccessor.duration_time( test_task.id);
	ASSERT_EQ(1300, result.count());

}

TEST(TimeAccessor, ChangeStartTime)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	task test_task( "test" );
	taskAccessor.create( test_task );
	time_accessor timeAccessor(tempdb);
	time_entry original( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	timeAccessor.create( original );
	auto te = timeAccessor.by_id(original.id);
	timeAccessor.update(te->with_start( system_clock::from_time_t( 300 )));
	auto result = timeAccessor.duration_time( test_task.id);
	ASSERT_EQ(700, result.count());

}

TEST(TimeAccessor, UpdateTime)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	Notify_observer observer(notifier);

	task_accessor taskAccessor(tempdb);
	task test_task( "test" );
	taskAccessor.create(test_task );
	time_accessor timeAccessor(tempdb);
	time_entry first( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	timeAccessor.create( first );
	observer.task_id_time = {};
	auto original = timeAccessor.by_id(first.id).value();
	timeAccessor.update( original.with_start( system_clock::from_time_t( 300 )).with_stop( system_clock::from_time_t( 700 )));
	auto result = timeAccessor.duration_time( test_task.id);
	ASSERT_EQ(400s, result );
	ASSERT_TRUE( observer.task_id_time.has_value());
	ASSERT_EQ(observer.task_id_time.value(), test_task.id );

}

TEST(TimeAccessor, RemoveTime)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	task test_task("test");
	taskAccessor.create( test_task );
	time_accessor timeAccessor(tempdb);
	time_entry te = time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	timeAccessor.create( te );
	timeAccessor.create( time_entry( test_task.id, system_clock::from_time_t( 2000 ), system_clock::from_time_t( 2300 )));
	timeAccessor.remove( te );
	auto result = timeAccessor.duration_time( test_task.id );
	ASSERT_EQ(300s, result);
}

TEST(TimeAccessor, GetLatestTasks)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);

	task test_task( "test" );
	taskAccessor.create( test_task );
	time_accessor timeAccessor(tempdb);
	timeAccessor.create( time_entry( test_task.id, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000)));
	auto result = timeAccessor.latest_active_tasks(10);
	ASSERT_EQ(1, result.size());
	ASSERT_EQ( test_task.id, result[0]);
	taskAccessor.remove( test_task.id);
	result = timeAccessor.latest_active_tasks(10);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, GetDetailTimeList)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	extended_task_accessor taskAccessor(tempdb);

	task test_task( "test" );
	taskAccessor.create(test_task );
	timeAccessor.create( time_entry( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 )) );
	vector<time_entry> result = timeAccessor.by_activity(
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
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	extended_task_accessor taskAccessor(tempdb);
	task test_task( "test" );
	taskAccessor.create( test_task );
	time_entry original( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 ));
	timeAccessor.create( original );
	auto te = timeAccessor.by_id(original.id);
	ASSERT_EQ( test_task.id, te->owner_id) << "Check task id";
	ASSERT_EQ(system_clock::from_time_t( 10 ), te->start) << "Check start";
	ASSERT_EQ(system_clock::from_time_t( 100 ), te->stop) << "Check stop";

}

TEST(TimeAccessor, testGetByUUID)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	extended_task_accessor taskAccessor(tempdb);

	task test_task( "test" );
	taskAccessor.create( test_task );
	time_entry original = time_entry( test_task.id, system_clock::from_time_t( 10 ), system_clock::from_time_t( 100 ));
	timeAccessor.create( original );

	auto te = timeAccessor.by_id(original.id);
	ASSERT_EQ( test_task.id, te->owner_id ) << "Check task id";
	ASSERT_EQ(system_clock::from_time_t( 10 ), te->start) << "Check start";
	ASSERT_EQ(system_clock::from_time_t( 100 ), te->stop) << "Check stop";
	ASSERT_EQ( original.id, te->id) << "Check id";

}




TEST(TimeAccessor, newItem)
{
	notification_manager notifier;
	TempDB db(notifier);
	time_accessor timeAccessor(db);
	extended_task_accessor taskAccessor(db);

	task test_task( "test" );
	taskAccessor.create( test_task );
	time_entry item1(
			time_id(),
			test_task.id,
			system_clock::from_time_t( 100 ),
			system_clock::from_time_t( 200 ),
			STOPPED,
			system_clock::from_time_t( 200 ),
			COMMENT);

	timeAccessor.create(item1);
	auto item2 = timeAccessor.by_id(item1.id);

	ASSERT_EQ( item1.id, item2->id) << "UUID: ";
	ASSERT_EQ( item1.owner_id, item2->owner_id) << "Task_ID: ";
	ASSERT_EQ(item1.start, item2->start) << "Start: ";
	ASSERT_EQ(item1.stop, item2->stop) << "Stop: ";
	ASSERT_EQ(item1.state, item2->state) << "Deleted: ";
	ASSERT_EQ(item1.changed, item2->changed) << "Changed: ";
	ASSERT_EQ(item1.comment, item2->comment) << "Comment: ";

	ASSERT_THROW(timeAccessor.create(item1), db_exception);

}

TEST(TimeAccessor, GetTotalTimeWithChildren)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	task_accessor taskAccessor(tempdb);

	task parent( "parent" );
	taskAccessor.create( parent );
	auto parentId = parent.id;
	task child( "test", parent.id );
	taskAccessor.create( child );
	auto taskId = child.id;
	timeAccessor.create( time_entry( taskId, system_clock::from_time_t( 4000 ), system_clock::from_time_t( 5000 )) );
	int parentTotalTime = timeAccessor.total_cumulative_time(parentId).count();
	ASSERT_EQ(1000, parentTotalTime);
	int childTotalTime = timeAccessor.total_cumulative_time(taskId).count();
	ASSERT_EQ(1000, childTotalTime);
}

TEST(TimeAccessor, getTimesChangedSince)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	task_accessor taskAccessor(tempdb);

	task test_task( "test" );
	auto taskId = test_task.id;
	taskAccessor.create( test_task );

	time_entry original( taskId, system_clock::from_time_t( 0 ), system_clock::from_time_t( 1000 ));
	timeAccessor.create( original );

	auto item = timeAccessor.by_id( original.id );
	vector<time_entry> result = timeAccessor.times_changed_since( system_clock::from_time_t(0) );
	ASSERT_EQ(1, result.size());
	result = timeAccessor.times_changed_since(item->changed);
	ASSERT_EQ(1, result.size());
	result = timeAccessor.times_changed_since(item->changed + 1s);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, getActiveTasks)
{
	notification_manager notifier;
	TempDB db(notifier);
	time_accessor timeAccessor (db);
	extended_task_accessor taskAccessor(db);


	task test_task( "test" );
	auto taskId = test_task.id;
	taskAccessor.create( test_task );
	time_entry item(
			time_id(),
			taskId,
			system_clock::from_time_t( 100 ),
			system_clock::from_time_t( 600 ),
			STOPPED,
			system_clock::from_time_t( 200 ),
			COMMENT);

	timeAccessor.create(item);

	auto result = timeAccessor.active_tasks(system_clock::from_time_t(0), system_clock::from_time_t(50000));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(system_clock::from_time_t(110), system_clock::from_time_t(500));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";


	result = timeAccessor.active_tasks(system_clock::from_time_t(90), system_clock::from_time_t(100));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(system_clock::from_time_t(600), system_clock::from_time_t(800));
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(system_clock::from_time_t(0), system_clock::from_time_t(99));
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(system_clock::from_time_t(601), system_clock::from_time_t(8000));
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	taskAccessor.remove(taskId);
	result = timeAccessor.active_tasks(system_clock::from_time_t(110), system_clock::from_time_t(500));
	ASSERT_EQ(0, result.size()) << "Deleted task is shown";

}


}
