#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TimeManagement.h"
#include "Database.h"

class TimeManagementTest
{
public:
	TimeManagementTest()
	{
		database = boost::shared_ptr<DB::Database>(new DB::Database("/tmp/test.db"));
		taskAccessor = database->getTaskAccessor();
		timeAccessor = database->getTimeAccessor();
		taskID = taskAccessor->newTask("task", 0);
		subTaskID = taskAccessor->newTask("subtask",taskID);
		timeAccessor->newTime(taskID, 100, 200);
		timeAccessor->newTime(subTaskID, 150, 200);
	}

	Task getTask(time_t start = 0, time_t stop = 0)
	{
		return taskAccessor->getTask(taskID, start, stop);
	}
private:
	int64_t taskID;
	int64_t subTaskID;
	boost::shared_ptr<DB::Database> database;
	boost::shared_ptr<ITaskAccessor> taskAccessor;
	boost::shared_ptr<ITimeAccessor> timeAccessor;
};

void tasksTimeIs100()
{
	TimeManagementTest tmt;
	Task task = tmt.getTask();
	ASSERT_EQUAL(100, task.getTime());
}

void tasksTotalTimeIs150()
{
	TimeManagementTest tmt;
	Task task = tmt.getTask();
	ASSERT_EQUAL(150, task.getTotalTime());
}

void managesTimeWhenEndIsAfterLimit()
{
	TimeManagementTest tmt;
	Task task = tmt.getTask(0,150);
	ASSERT_EQUAL(50, task.getTime());
}

//Manage time when start is before limit
//ManageTime completely inside limits
//ManageTotalTime completely inside limits

cute::suite make_suite_test_TimeManagement()
{
	cute::suite s;
	s.push_back(CUTE(tasksTimeIs100));
	s.push_back(CUTE(tasksTotalTimeIs150));
	s.push_back(CUTE(managesTimeWhenEndIsAfterLimit));
	return s;
}

