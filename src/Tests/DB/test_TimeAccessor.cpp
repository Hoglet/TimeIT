/*
 * testTimeAcessor.cpp
 *
 *  Created on: 7 Oct 2012
 *      Author: hoglet
 */

#include "test_TimeAccessor.h"
#include "cute.h"
#include "TempDB.h"

namespace Test
{

void simpleTest()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(1000, result);
}

void testChangeEndTime()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->changeEndTime(timeId,1300);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(1300, result);

}

void testChangeStartTime()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->changeStartTime(timeId,300);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(700, result);

}

void testUpdateTime()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->updateTime(timeId,300,700);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(400, result);

}

void testRemoveTime()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	int64_t timeId = timeAccessor->newTime(taskId, 0, 1000);
	timeAccessor->newTime(taskId, 2000, 2300);
	timeAccessor->remove(timeId);
	int result = timeAccessor->getTime(taskId, 0, 0);
	ASSERT_EQUAL(300, result);
}

void testGetLatestTasks()
{
	TempDB tempdb;
	const int64_t taskId = 1;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	timeAccessor->newTime(taskId, 0, 1000);
	std::vector<int64_t> result = timeAccessor->getLatestTasks(10);
	ASSERT_EQUAL(1, result.size());
	ASSERT_EQUAL(taskId,result[0]);
}

void testGetDetailTimeList()
{
	TempDB tempdb;
	std::shared_ptr<ITimeAccessor> timeAccessor = tempdb.getTimeAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	const int64_t taskId = taskAccessor->newTask("test", 0);
	timeAccessor->newTime(taskId, 10, 100);
	std::vector<TimeEntry>  result = timeAccessor->getDetailTimeList(taskId, 0, 10000);
	ASSERT_EQUAL(1, result.size());
	TimeEntry te = result[0];
	ASSERT_EQUAL(10, te.start);
	ASSERT_EQUAL(100, te.stop);
}



cute::suite make_suite_TimeAccessor_test()
{
	cute::suite s;
	s.push_back(CUTE(simpleTest));
	s.push_back(CUTE(testChangeEndTime));
	s.push_back(CUTE(testChangeStartTime));
	s.push_back(CUTE(testUpdateTime));
	s.push_back(CUTE(testGetLatestTasks));
	s.push_back(CUTE(testGetDetailTimeList));
	s.push_back(CUTE(testRemoveTime));
	return s;
}

} /* namespace test_TimeAcessor */
