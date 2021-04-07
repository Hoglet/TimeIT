#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/exceptions/db_exception.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/extended_task_accessor.h>
#include "notify_observer.h"

namespace test
{


using namespace libtimeit;
using namespace std;

TEST(TimeAccessor, simpleTest)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	timeAccessor.create( Time_entry( taskId, 0, 1000 ) );
	int result = timeAccessor.duration_time(taskId, 0, 0);
	ASSERT_EQ(1000, result);
}

TEST(TimeAccessor, ChangeEndTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_accessor timeAccessor(tempdb);
	int64_t timeId = timeAccessor.create( Time_entry( taskId, 0, 1000 ) );
	auto te = timeAccessor.by_ID(timeId);
	if (te)
	{
		timeAccessor.update(te->with_stop(1300));
	}
	int result = timeAccessor.duration_time(taskId, 0, 0);
	ASSERT_EQ(1300, result);

}

TEST(TimeAccessor, ChangeStartTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_accessor timeAccessor(tempdb);
	int64_t timeId = timeAccessor.create( Time_entry( taskId, 0, 1000 ) );
	auto te = timeAccessor.by_ID(timeId);
	timeAccessor.update(te->with_start(300));
	int result = timeAccessor.duration_time(taskId, 0, 0);
	ASSERT_EQ(700, result);

}

TEST(TimeAccessor, UpdateTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Notify_observer observer(notifier);

	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_accessor timeAccessor(tempdb);
	int64_t timeId = timeAccessor.create( Time_entry( taskId, 0, 1000 ) );
	observer.task_id_time = 0;
	auto original = timeAccessor.by_ID(timeId).value();
	timeAccessor.update( original.with_start(300).with_stop(700));
	int result = timeAccessor.duration_time(taskId, 0, 0);
	ASSERT_EQ(400, result);
	ASSERT_EQ(observer.task_id_time, taskId );

}

TEST(TimeAccessor, RemoveTime)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_accessor timeAccessor(tempdb);
	int64_t timeId = timeAccessor.create( Time_entry( taskId, 0, 1000 ));
	timeAccessor.create( Time_entry( taskId, 2000, 2300 ));
	timeAccessor.remove(timeId);
	int result = timeAccessor.duration_time(taskId, 0, 0);
	ASSERT_EQ(300, result);
}

TEST(TimeAccessor, GetLatestTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_accessor timeAccessor(tempdb);
	timeAccessor.create( Time_entry(taskId, 0, 1000) );
	vector<int64_t> result = timeAccessor.latest_active_tasks(10);
	ASSERT_EQ(1, result.size());
	ASSERT_EQ(taskId, result[0]);
	taskAccessor.remove(taskId);
	result = timeAccessor.latest_active_tasks(10);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, GetDetailTimeList)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Extended_task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	timeAccessor.create( Time_entry( taskId, 10, 100) );
	vector<Time_entry> result = timeAccessor.time_list(taskId, 0, 10000);
	ASSERT_EQ(1, result.size());
	Time_entry te = result[0];
	ASSERT_EQ(10, te.start());
	ASSERT_EQ(100, te.stop());
}

TEST(TimeAccessor, testGetByID)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Extended_task_accessor taskAccessor(tempdb);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	int64_t timeEntryID = timeAccessor.create( Time_entry(taskId, 10, 100) );
	auto te = timeAccessor.by_ID(timeEntryID);
	ASSERT_EQ(taskId, te->task_ID()) << "Check task ID";
	ASSERT_EQ(10, te->start()) << "Check start";
	ASSERT_EQ(100, te->stop()) << "Check stop";

}

TEST(TimeAccessor, newItem)
{
	Notifier notifier;
	TempDB db(notifier);
	Time_accessor timeAccessor(db);
	Extended_task_accessor taskAccessor(db);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_entry item1(0, UUID(), taskId, {}, 100, 200, false, STOPPED, 200);

	int64_t timeEntryID = timeAccessor.create(item1);
	auto item2 = timeAccessor.by_ID(timeEntryID);

	ASSERT_EQ(item1.get_UUID(), item2->get_UUID()) << "UUID: ";
	ASSERT_EQ(item1.task_ID(), item2->task_ID()) << "Task_ID: ";
	ASSERT_EQ(item1.start(), item2->start()) << "Start: ";
	ASSERT_EQ(item1.stop(), item2->stop()) << "Stop: ";
	ASSERT_EQ(item1.deleted(), item2->deleted()) << "Deleted: ";
	ASSERT_EQ(item1.running(), item2->running()) << "Running: ";
	ASSERT_EQ(item1.changed(), item2->changed()) << "Changed: ";

	ASSERT_THROW(timeAccessor.create(item1), db_exception);
//	ASSERT_EQUALM("Updating with identical item ", false, timeAccessor->update(item1));

//	TimeEntry(	int64_t id, std::string uuid, int64_t taskID, std::string task_UUID, time_t start, time_t stop, bool deleted, bool running, int64_t changed);

//	TimeEntry item2( item1.getID(), item1.get_UUID(), item1.getTaskID(), item1.getTaskUUID(), item1.getStart(),   )

	//TimeEntry faultyItem1(0, uuidManager.randomUUID(), taskId + 1, "", 100, 200, false, false, 200);

}

TEST(TimeAccessor, GetTotalTimeWithChildren)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Task_accessor taskAccessor(tempdb);
	const int64_t parentId = taskAccessor.create(Task("test", 0));
	const int64_t taskId = taskAccessor.create(Task("test", parentId));
	timeAccessor.create( Time_entry(taskId, 4000, 5000) );
	int parentTotalTime = timeAccessor.total_cumulative_time(parentId, 0, 0);
	ASSERT_EQ(1000, parentTotalTime);
	int childTotalTime = timeAccessor.total_cumulative_time(taskId, 0, 0);
	ASSERT_EQ(1000, childTotalTime);
}

TEST(TimeAccessor, getTimesChangedSince)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Time_accessor timeAccessor(tempdb);
	Task_accessor taskAccessor(tempdb);

	const int64_t taskId = taskAccessor.create(Task("test", 0));
	int64_t timeid = timeAccessor.create( Time_entry(taskId, 0, 1000) );

	auto item = timeAccessor.by_ID(timeid);
	vector<Time_entry> result = timeAccessor.times_changed_since(0);
	ASSERT_EQ(1, result.size());
	result = timeAccessor.times_changed_since(item->changed());
	ASSERT_EQ(1, result.size());
	result = timeAccessor.times_changed_since(item->changed() + 1);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, getActiveTasks)
{
	Notifier notifier;
	TempDB db(notifier);
	Time_accessor timeAccessor (db);
	Extended_task_accessor taskAccessor(db);
	const int64_t taskId = taskAccessor.create(Task("test", 0));
	Time_entry item(0, UUID(), taskId, {}, 100, 600, false, STOPPED, 200);

	timeAccessor.create(item);

	vector<long int> result = timeAccessor.active_tasks(0, 50000);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(110, 500);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";


	result = timeAccessor.active_tasks(90, 100);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(600, 800);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(0, 99);
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	result = timeAccessor.active_tasks(601, 8000);
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	taskAccessor.remove(taskId);
	result = timeAccessor.active_tasks(110, 500);
	ASSERT_EQ(0, result.size()) << "Deleted task is shown";

}


}