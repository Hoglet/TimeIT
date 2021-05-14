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
	Notifier notifier;
	TempDB tempdb(notifier);
	Extended_task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	int64_t taskId2 = taskAccessor.create(Task("Test2", taskId));

	Time_accessor timeAccessor(tempdb);
	timeAccessor.create( Time_entry( taskId, 0, 1000) );
	timeAccessor.create( Time_entry( taskId2, 0, 1000 ) );

	auto task = taskAccessor.by_id(taskId);
	ASSERT_EQ("Test", task->name);
	ASSERT_EQ(2000, task->total_time);

	auto task2 = taskAccessor.by_id(taskId2);
	ASSERT_EQ("Test2", task2->name);
	ASSERT_EQ(1000, task2->total_time);
}

TEST(ExtendedTaskAccessor, by_parent_ID)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Extended_task_accessor taskAccessor(tempdb);
	Time_accessor timeAccessor(tempdb);

	int64_t taskId = taskAccessor.create(Task("Test", 0));

	vector<Extended_task> tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(1, tasks.size());

	taskAccessor.create(Task("NextTask", 0));
	tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(2, tasks.size());

	int64_t taskId2 = taskAccessor.create(Task("Test2", taskId));
	timeAccessor.create( Time_entry( taskId2, 0, 1000 ) );

	tasks = taskAccessor.by_parent_id();
	Extended_task &task = tasks.at(0);
	ASSERT_EQ("Test", task.name);
	ASSERT_EQ(1000, task.total_time);
}

TEST(ExtendedTaskAccessor, testTotalTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Extended_task_accessor taskAccessor(tempdb );
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	int64_t taskId2 = taskAccessor.create(Task("Test2", taskId));

	Time_accessor timeAccessor(tempdb);
	timeAccessor.create( Time_entry( taskId2, 0, 1000 ));

	auto task = taskAccessor.by_id(taskId);
	ASSERT_EQ(1000, task->total_time);
}

TEST(ExtendedTaskAccessor, setExpandedTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Extended_task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	auto task = taskAccessor.by_id(taskId);
	ASSERT_EQ(false, task->expanded);
	taskAccessor.set_task_expanded(taskId, true);
	auto task2 = taskAccessor.by_id(taskId);
	ASSERT_EQ(true, task2->expanded);
}

TEST(ExtendedTaskAccessor, testTimeReporting)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Extended_task_accessor taskAccessor(tempdb);
	const int64_t parentId = taskAccessor.create(Task("test", 0));
	const int64_t taskId = taskAccessor.create(Task("test", parentId));
	timeAccessor.create( Time_entry( taskId, 4000, 5000 ) );

	auto tasks = taskAccessor.by_parent_id(parentId);
	Extended_task task = tasks.at(0);
	int result = task.total_time;
	ASSERT_EQ(1000, result);

	auto task2 = taskAccessor.by_id(taskId);
	result = task2->total_time;
	ASSERT_EQ(1000, result);

	tasks = taskAccessor.by_parent_id(0, 0, 0);
	auto task3 = tasks.at(0);
	result = task.total_time;
	ASSERT_EQ(1000, result);

	tasks = taskAccessor.by_parent_id(parentId, 0, 0);
	auto task4 = tasks.at(0);
	result = task4.total_time;
	ASSERT_EQ(1000, result);

}

}