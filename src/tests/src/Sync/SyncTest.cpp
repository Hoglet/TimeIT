#include <gtest/gtest.h>
#include "libtimeit/sync/network.h"
#include "libtimeit/sync/json_converter.h"
#include "../Mock/MockNetwork.h"
#include "libtimeit/timer.h"
#include "libtimeit/sync/sync_manager.h"
#include "../DB/TempDB.h"

namespace test
{
using namespace std;
using namespace libtimeit;

TEST( SyncManager, fullSyncEmptyClient )
{
	Notifier    notifier;
	TempDB      db (notifier);
	MockNetwork network;

	Settings_accessor settings(db);
	settings.set_string("Username", "testman");
	settings.set_string("URL", "localhost");

	Timer timer;
	Sync_manager syncManager(db, network, notifier, timer);

	std::string taskKey = "/tasks/testman/0";
	std::string taskResponse = "[{\"name\": \"Child\", "
			"\"id\": \"00b3015e-00d6-418e-81c8-0125012d0172\", "
			"\"parent\": {\"id\": \"013900e6-00dd-40f7-b0d6-00de00bf006b\"}, "
			"\"lastChange\": 1375358093, "
			"\"completed\": false,"
			"\"deleted\": true},"
			"{\"name\": \"Parent\", "
			"\"id\": \"013900e6-00dd-40f7-b0d6-00de00bf006b\", "
			"\"lastChange\": 1375358076, "
			"\"deleted\": false, "
			"\"completed\": false}]";
	std::string timesKey = "/times/testman/0";

	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";

	std::string timesResponse = "[{\"id\": \"010c012c-00b9-40f6-80dd-018e011d0191\","
			"\"task\":{\"id\":\"00b3015e-00d6-418e-81c8-0125012d0172\"},"
			"\"start\": 1363597429, \"stop\": 1363597541, \"deleted\": false,"
			"\"changed\": 1376388171}]";
	network.setResponse(taskKey, taskResponse);
	network.setResponse(timesKey, timesResponse);


	while(syncManager.status() == Sync_state::IDLE)
	{
		timer.on_signal_1_second();
	}
	while(syncManager.status() != Sync_state::IDLE)
	{
		timer.on_signal_1_second();
	}

	Task_accessor taskAccessor(db);
	vector<Task> tasks = taskAccessor.changed_since();
	ASSERT_EQ( 2, tasks.size()) << "Checking amount of tasks in database";
	for (Task task : tasks)
	{
		if (task.name == "Parent")
		{
			ASSERT_EQ( 1375358076, task.last_changed) << "Checking Parent's change time";
		}
		else if (task.name == "Child")
		{
			string parentUUID = "013900e6-00dd-40f7-b0d6-00de00bf006b";
			ASSERT_EQ( 1375358093, task.last_changed) << "Checking Child's change time";
			ASSERT_EQ( parentUUID, task.parent_uuid->c_str()) << "Checking Child's parent ";
		}
		else
		{
			FAIL( ) << "Unknown task in list";
		}
	}
	Time_accessor timeAccessor(db);
	Time_list times = timeAccessor.times_changed_since();
	ASSERT_EQ( 1, times.size()) << "Checking amount of times in database";
	for (auto item : times)
	{
		ASSERT_EQ( 1, item.id) << "TimeEntry ID";
		ASSERT_STREQ( "010c012c-00b9-40f6-80dd-018e011d0191", item.uuid.c_str()) << "TimeEntry UUID";
		ASSERT_STREQ( "00b3015e-00d6-418e-81c8-0125012d0172", item.task_uuid->c_str()) << "TimeEntry taskUUD";
		ASSERT_EQ( 1363597429, item.start) << "TimeEntry start";
		ASSERT_EQ( 1363597541, item.stop) << "TimeEntry stop";
		ASSERT_EQ( 1376388171, item.changed) << "TimeEntry changed";
		ASSERT_EQ( STOPPED, item.state) << "TimeEntry deleted";
	}

	syncManager.reset();
	while(syncManager.status() == Sync_state::IDLE)
	{
		timer.on_signal_1_second();
	}
	while(syncManager.status() != Sync_state::IDLE)
	{
		timer.on_signal_1_second();
	}

	tasks = taskAccessor.changed_since();
	times = timeAccessor.times_changed_since();
	ASSERT_EQ( 2, tasks.size()) << "Checking number of tasks after repeat syncing ";
	ASSERT_EQ( 1, times.size()) << "Checking number of times after repeat syncing ";
}


} /* namespace test */
