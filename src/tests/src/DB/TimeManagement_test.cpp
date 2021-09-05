#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/extended_task_accessor.h>

namespace test
{
using namespace libtimeit;

extended_task getTask()
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	extended_task_accessor taskAccessor(tempdb);
	time_accessor timeAccessor(tempdb);
	task test_task( "test" );
	auto taskID = test_task.id;
	taskAccessor.create( test_task);
	task sub_task( "sub_task", taskID);
	auto subTaskID = sub_task.id;
	taskAccessor.create( sub_task );
	timeAccessor.create( time_entry( taskID, system_clock::from_time_t( 100 ), system_clock::from_time_t( 200 )) );
	timeAccessor.create( time_entry( subTaskID, system_clock::from_time_t( 150 ), system_clock::from_time_t( 200 )) );
	return *taskAccessor.by_id(taskID);
}

TEST(TimeManagement, tasksTimeIs100)
{
	extended_task task1 = getTask();
	ASSERT_EQ(100s, task1.time);
}

TEST(TimeManagement, tasksTotalTimeIs150)
{
	extended_task task1 = getTask();
	ASSERT_EQ(150s, task1.total_time);
}


}