#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/extended_task_accessor.h>
#include <libtimeit/exceptions/db_exception.h>

namespace test
{


using namespace libtimeit;
using namespace std;

TEST(ExtendedTaskAccessor, by_ID)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb);

	task parent( "Test", {});
	task test_task_2( "Test2", parent.id);
	taskAccessor.create(parent);
	taskAccessor.create(test_task_2);

	time_accessor timeAccessor(tempdb);
	timeAccessor.create( time_entry( parent.id, 0, 1000) );
	timeAccessor.create( time_entry( test_task_2.id, 0, 1000 ) );

	auto task1 = taskAccessor.by_id(parent.id);
	ASSERT_EQ("Test", task1->name);
	ASSERT_EQ(2000, task1->total_time);

	auto task2 = taskAccessor.by_id(test_task_2.id);
	ASSERT_EQ("Test2", task2->name);
	ASSERT_EQ(1000, task2->total_time);
}

TEST(ExtendedTaskAccessor, by_parent_ID)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb);
	time_accessor timeAccessor(tempdb);

	task test_task( "Test", {} );
	taskAccessor.create( test_task );

	vector<extended_task> tasks = taskAccessor.by_parent_id({});
	ASSERT_EQ(1, tasks.size());

	task next_task("NextTask", {});
	taskAccessor.create(next_task);
	tasks = taskAccessor.by_parent_id({});
	ASSERT_EQ(2, tasks.size());

	task test_task_2("Test2", test_task.id);
	taskAccessor.create(test_task_2);
	timeAccessor.create( time_entry( test_task_2.id, 0, 1000 ) );

	tasks = taskAccessor.by_parent_id({});
	extended_task &task1 = tasks.at(0);
	ASSERT_EQ("Test", task1.name);
	ASSERT_EQ(1000, task1.total_time);
}

TEST(ExtendedTaskAccessor, testTotalTime)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb );
	task test_task_1("Test", {});
	task test_task_2("Test2", test_task_1.id);
	taskAccessor.create( test_task_1 );
	taskAccessor.create(test_task_2);

	time_accessor timeAccessor(tempdb);
	timeAccessor.create( time_entry( test_task_2.id, 0, 1000 ));

	auto task1 = taskAccessor.by_id(test_task_1.id);
	ASSERT_EQ(1000, task1->total_time);
}

TEST(ExtendedTaskAccessor, setExpandedTasks)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb);

	task test_task("Test", {});
	taskAccessor.create(test_task);
	auto taskId = test_task.id;
	auto task1 = taskAccessor.by_id(taskId);
	ASSERT_EQ(false, task1->expanded);
	taskAccessor.set_task_expanded(taskId, true);
	auto task2 = taskAccessor.by_id(taskId);
	ASSERT_EQ(true, task2->expanded);
}

TEST(ExtendedTaskAccessor, testTimeReporting)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	time_accessor timeAccessor(tempdb);
	extended_task_accessor taskAccessor(tempdb);
	task parent("test", {});
	taskAccessor.create( parent );
	auto parentId = parent.id;
	task test_task("test", parentId);
	taskAccessor.create(test_task);
	auto taskId = test_task.id;
	timeAccessor.create( time_entry( taskId, 4000, 5000 ) );

	auto tasks = taskAccessor.by_parent_id(parentId);
	extended_task task1 = tasks.at(0);
	int result = task1.total_time;
	ASSERT_EQ(1000, result);

	auto task2 = taskAccessor.by_id(taskId);
	result = task2->total_time;
	ASSERT_EQ(1000, result);

	tasks = taskAccessor.by_parent_id({});
	auto task3 = tasks.at(0);
	result = task1.total_time;
	ASSERT_EQ(1000, result);

	tasks = taskAccessor.by_parent_id(parentId);
	auto task4 = tasks.at(0);
	result = task4.total_time;
	ASSERT_EQ(1000, result);

}

}