#include "test_TimeAccessor.h"
#include "cute.h"
#include "TempDB.h"
#include "Utils.h"

using namespace DB;

namespace Test
{

void TimeAccessor_simpleTest()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	timeAccessor->newTime(taskId, 0, 1000);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(1000, result);
}

void TimeAccessor_ChangeEndTime()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	TimeEntry te = 	timeAccessor->getByID(timeId);
	te.setStop(1300);
	timeAccessor->update(te);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(1300, result);

}

void TimeAccessor_ChangeStartTime()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	TimeEntry te = timeAccessor->getByID(timeId);
	te.setStart(300);
	timeAccessor->update(te);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(700, result);

}

void TimeAccessor_UpdateTime()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->updateTime(timeId, 300, 700);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(400, result);

}

void TimeAccessor_RemoveTime()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->newTime(taskId, 2000, 2300);
	timeAccessor->remove(timeId);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(300, result);
}

void TimeAccessor_GetLatestTasks()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	std::vector<int64_t> result = timeAccessor->getLatestTasks(10);
	ASSERT_EQUAL(1, result.size());
	ASSERT_EQUAL(taskId, result[0]);
}

void TimeAccessor_GetDetailTimeList()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	timeAccessor->newTime(taskId, 10, 100);
	std::vector<TimeEntry> result = timeAccessor->getDetailTimeList(taskId, 0, 10000);
	ASSERT_EQUAL(1, result.size());
	TimeEntry te = result[0];
	ASSERT_EQUAL(10, te.getStart());
	ASSERT_EQUAL(100, te.getStop());
}

void TimeAccessor_testGetByID()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	int64_t timeEntryID = timeAccessor->newTime(taskId, 10, 100);
	TimeEntry te = timeAccessor->getByID(timeEntryID);
	ASSERT_EQUALM("Check task ID", taskId, te.getTaskID());
	ASSERT_EQUALM("Check start", 10, te.getStart());
	ASSERT_EQUALM("Check stop", 100, te.getStop());

}

void TimeAccessor_newItem()
{
	TempDB db;
	std::shared_ptr<ITimeAccessor> timeAccessor = db.getTimeAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = db.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	TimeEntry item1(0, UUID::randomUUID(), taskId, "", 100, 200, false, false, 200);

	int64_t timeEntryID = timeAccessor->newEntry(item1);
	TimeEntry item2 = timeAccessor->getByID(timeEntryID);

	ASSERT_EQUALM("UUID: ", item1.getUUID(), item2.getUUID());
	ASSERT_EQUALM("TaskID: ", item1.getTaskID(), item2.getTaskID());
	ASSERT_EQUALM("Start: ", item1.getStart(), item2.getStart());
	ASSERT_EQUALM("Stop: ", item1.getStop(), item2.getStop());
	ASSERT_EQUALM("Deleted: ", item1.getDeleted(), item2.getDeleted());
	ASSERT_EQUALM("Running: ", item1.getRunning(), item2.getRunning());
	ASSERT_EQUALM("Changed: ", item1.getLastChanged(), item2.getLastChanged());

	ASSERT_THROWSM("Adding existing", timeAccessor->newEntry(item1), dbexception);
//	ASSERT_EQUALM("Updating with identical item ", false, timeAccessor->update(item1));

//	TimeEntry(	int64_t id, std::string uuid, int64_t taskID, std::string taskUUID, time_t start, time_t stop, bool deleted, bool running, int64_t changed);

//	TimeEntry item2( item1.getID(), item1.getUUID(), item1.getTaskID(), item1.getTaskUUID(), item1.getStart(),   )

	//TimeEntry faultyItem1(0, uuidManager.randomUUID(), taskId + 1, "", 100, 200, false, false, 200);

}

void TimeAccessor_GetTotalTimeWithChildren_test()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t parentId = taskAccessor->newTask("test", 0);
	const int64_t taskId = taskAccessor->newTask("test", parentId);
	timeAccessor->newTime(taskId, 4000, 5000);
	int parentTotalTime = timeAccessor->getTotalTimeWithChildren(parentId, 0, 0);
	ASSERT_EQUAL(1000, parentTotalTime);
	int childTotalTime = timeAccessor->getTotalTimeWithChildren(taskId, 0, 0);
	ASSERT_EQUAL(1000, childTotalTime);
}

void TimeAccessor_getTimesChangedSince()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();

	const int64_t taskId = taskAccessor->newTask("test", 0);
	int64_t timeid=timeAccessor->newTime(taskId, 0, 1000);

	TimeEntry item = timeAccessor->getByID(timeid);
	std::shared_ptr<std::vector<TimeEntry> > result = timeAccessor->getTimesChangedSince(0);
	ASSERT_EQUAL(1, (*result).size());
	result = timeAccessor->getTimesChangedSince(item.getLastChanged());
	ASSERT_EQUAL(1, (*result).size());
	result = timeAccessor->getTimesChangedSince(item.getLastChanged()+1);
	ASSERT_EQUAL(0, (*result).size());

}

void TimeAccessor_removeShortTimeSpans()
{
	TempDB db;
	std::shared_ptr<TimeAccessor> timeAccessor = std::dynamic_pointer_cast<TimeAccessor>(db.getTimeAccessor());
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = db.getExtendedTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	TimeEntry item1(0, UUID::randomUUID(), taskId, "", 100, 110, false, false, 200);
	TimeEntry item2(0, UUID::randomUUID(), taskId, "", 100, 160, false, false, 200);

	timeAccessor->newEntry(item1);
	timeAccessor->newEntry(item2);
	timeAccessor->removeShortTimeSpans();

	std::vector<TimeEntry> items = timeAccessor->getDetailTimeList(taskId, 0, 1000);

	ASSERT_EQUAL(1, items.size());
}


cute::suite make_suite_TimeAccessor_test()
{
	cute::suite s;
	s.push_back(CUTE(TimeAccessor_removeShortTimeSpans));
	s.push_back(CUTE(TimeAccessor_getTimesChangedSince));
	s.push_back(CUTE(TimeAccessor_simpleTest));
	s.push_back(CUTE(TimeAccessor_ChangeEndTime));
	s.push_back(CUTE(TimeAccessor_ChangeStartTime));
	s.push_back(CUTE(TimeAccessor_UpdateTime));
	s.push_back(CUTE(TimeAccessor_GetLatestTasks));
	s.push_back(CUTE(TimeAccessor_GetDetailTimeList));
	s.push_back(CUTE(TimeAccessor_RemoveTime));
	s.push_back(CUTE(TimeAccessor_testGetByID));
	s.push_back(CUTE(TimeAccessor_newItem));
	s.push_back(CUTE(TimeAccessor_GetTotalTimeWithChildren_test));
	return s;
}


} /* namespace test_TimeAcessor */
