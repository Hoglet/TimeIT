#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/exceptions/dbexception.h>

using namespace DB;

TEST(TimeAccessor, simpleTest)
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	timeAccessor->newTime(taskId, 0, 1000);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQ(1000, result);
}

TEST(TimeAccessor, ChangeEndTime)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	auto te = timeAccessor->getByID(timeId);
	if(te)
	{
		timeAccessor->update(te->withStop(1300));
	}
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQ(1300, result);

}

TEST(TimeAccessor, ChangeStartTime)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	auto te = timeAccessor->getByID(timeId);
	timeAccessor->update(te->withStart(300));
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQ(700, result);

}

TEST(TimeAccessor, UpdateTime)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->updateTime(timeId, 300, 700);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQ(400, result);

}

TEST(TimeAccessor, RemoveTime)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->newTime(taskId, 2000, 2300);
	timeAccessor->remove(timeId);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQ(300, result);
}

TEST(TimeAccessor, GetLatestTasks)
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	std::vector<int64_t> result = timeAccessor->getLatestTasks(10);
	ASSERT_EQ(1, result.size());
	ASSERT_EQ(taskId, result[0]);
	taskAccessor->removeTask(taskId);
	result = timeAccessor->getLatestTasks(10);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, GetDetailTimeList)
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	timeAccessor->newTime(taskId, 10, 100);
	std::vector<TimeEntry> result = timeAccessor->getDetailTimeList(taskId, 0, 10000);
	ASSERT_EQ(1, result.size());
	TimeEntry te = result[0];
	ASSERT_EQ(10, te.start());
	ASSERT_EQ(100, te.stop());
}

TEST(TimeAccessor, testGetByID)
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	int64_t timeEntryID = timeAccessor->newTime(taskId, 10, 100);
	auto te = timeAccessor->getByID(timeEntryID);
	ASSERT_EQ(taskId, te->taskID()) << "Check task ID";
	ASSERT_EQ(10, te->start()) << "Check start";
	ASSERT_EQ(100, te->stop()) << "Check stop";

}

TEST(TimeAccessor, newItem)
{
	TempDB db;
	std::shared_ptr<ITimeAccessor> timeAccessor = db.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = db.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	TimeEntry item1(0, UUID(), taskId, {}, 100, 200, false, false, 200);

	int64_t timeEntryID = timeAccessor->newEntry(item1);
	auto item2 = timeAccessor->getByID(timeEntryID);

	ASSERT_EQ(item1.getUUID(), item2->getUUID()) << "UUID: ";
	ASSERT_EQ(item1.taskID(), item2->taskID()) << "TaskID: ";
	ASSERT_EQ(item1.start(), item2->start()) << "Start: ";
	ASSERT_EQ(item1.stop(), item2->stop()) << "Stop: ";
	ASSERT_EQ(item1.deleted(), item2->deleted()) << "Deleted: ";
	ASSERT_EQ(item1.running(), item2->running()) << "Running: ";
	ASSERT_EQ(item1.changed(), item2->changed()) << "Changed: ";

	ASSERT_THROW(timeAccessor->newEntry(item1), dbexception);
//	ASSERT_EQUALM("Updating with identical item ", false, timeAccessor->update(item1));

//	TimeEntry(	int64_t id, std::string uuid, int64_t taskID, std::string taskUUID, time_t start, time_t stop, bool deleted, bool running, int64_t changed);

//	TimeEntry item2( item1.getID(), item1.getUUID(), item1.getTaskID(), item1.getTaskUUID(), item1.getStart(),   )

	//TimeEntry faultyItem1(0, uuidManager.randomUUID(), taskId + 1, "", 100, 200, false, false, 200);

}

TEST(TimeAccessor, GetTotalTimeWithChildren)
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t parentId = taskAccessor->newTask("test", 0);
	const int64_t taskId = taskAccessor->newTask("test", parentId);
	timeAccessor->newTime(taskId, 4000, 5000);
	int parentTotalTime = timeAccessor->getTotalTimeWithChildren(parentId, 0, 0);
	ASSERT_EQ(1000, parentTotalTime);
	int childTotalTime = timeAccessor->getTotalTimeWithChildren(taskId, 0, 0);
	ASSERT_EQ(1000, childTotalTime);
}

TEST(TimeAccessor, getTimesChangedSince)
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();

	const int64_t taskId = taskAccessor->newTask("test", 0);
	int64_t timeid = timeAccessor->newTime(taskId, 0, 1000);

	auto item = timeAccessor->getByID(timeid);
	std::vector<TimeEntry> result = timeAccessor->getTimesChangedSince(0);
	ASSERT_EQ(1, result.size());
	result = timeAccessor->getTimesChangedSince(item->changed());
	ASSERT_EQ(1, result.size());
	result = timeAccessor->getTimesChangedSince(item->changed() + 1);
	ASSERT_EQ(0, result.size());
}

TEST(TimeAccessor, getActiveTasks)
{
	TempDB db;
	std::shared_ptr<TimeAccessor> timeAccessor = std::dynamic_pointer_cast<TimeAccessor>(db.getTimeAccessor());
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = db.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	TimeEntry item(0, UUID(), taskId, {}, 100, 600, false, false, 200);

	timeAccessor->newEntry(item);

	std::vector<long int> result = timeAccessor->getActiveTasks(0, 50000);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor->getActiveTasks(110, 500);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";


	result = timeAccessor->getActiveTasks(90, 100);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor->getActiveTasks(600, 800);
	ASSERT_EQ(1, result.size()) << "Number of tasks are wrong";

	result = timeAccessor->getActiveTasks(0, 99);
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	result = timeAccessor->getActiveTasks(601, 8000);
	ASSERT_EQ(0, result.size()) << "Number of tasks are wrong";

	taskAccessor->removeTask(taskId);
	result = timeAccessor->getActiveTasks(110, 500);
	ASSERT_EQ(0, result.size()) << "Deleted task is shown";

}

TEST(TimeAccessor, removeShortTimeSpans)
{
	TempDB db;
	std::shared_ptr<TimeAccessor> timeAccessor = std::dynamic_pointer_cast<TimeAccessor>(db.getTimeAccessor());
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = db.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	TimeEntry item1(0, UUID(), taskId, {}, 100, 110, false, false, 200);
	TimeEntry item2(0, UUID(), taskId, {}, 100, 160, false, false, 200);

	timeAccessor->newEntry(item1);
	timeAccessor->newEntry(item2);
	timeAccessor->removeShortTimeSpans();

	std::vector<TimeEntry> items = timeAccessor->getDetailTimeList(taskId, 0, 1000);

	ASSERT_EQ(1, items.size());
}


