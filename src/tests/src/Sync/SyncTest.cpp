#include <gtest/gtest.h>
#include "libtimeit/sync/Network.h"
#include "libtimeit/sync/Json.h"
#include "../Mock/MockDatabase.h"
#include "../Mock/MockNetwork.h"
#include "libtimeit/Timer.h"
#include "libtimeit/sync/SyncManager.h"
#include "../DB/TempDB.h"

namespace test
{
using namespace std;
using namespace libtimeit;

TEST( SyncManager, fullSyncEmptyClient )
{
	Notifier notifier;
	shared_ptr<IDatabase> db = shared_ptr<IDatabase>(new TempDB(notifier));
	shared_ptr<MockNetwork> mockNetwork = shared_ptr<MockNetwork>(new MockNetwork());
	shared_ptr<INetwork> network = static_pointer_cast<INetwork>(mockNetwork);

	shared_ptr<ISettingsAccessor> settings = db->getSettingsAccessor();
	settings->SetStringByName("Username", "testman");
	settings->SetStringByName("URL", "localhost");

	Timer timer;
	SyncManager syncManager(db, network, notifier, timer);

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
	mockNetwork->setResponse(taskKey, taskResponse);
	mockNetwork->setResponse(timesKey, timesResponse);


	while(syncManager.status() == SyncState::IDLE)
	{
		timer.on_signal_1_second();
	}
	while(syncManager.status() != SyncState::IDLE)
	{
		timer.on_signal_1_second();
	}

	shared_ptr<ITaskAccessor> taskAccessor = db->getTaskAccessor();
	shared_ptr<vector<Task> > tasks = taskAccessor->getTasksChangedSince();
	ASSERT_EQ( 2, tasks->size()) << "Checking amount of tasks in database";
	for (Task task : *tasks)
	{
		if (task.name() == "Parent")
		{
			ASSERT_EQ( 1375358076, task.lastChanged()) << "Checking Parent's change time";
		}
		else if (task.name() == "Child")
		{
			string parentUUID = "013900e6-00dd-40f7-b0d6-00de00bf006b";
			ASSERT_EQ( 1375358093, task.lastChanged()) << "Checking Child's change time";
			ASSERT_EQ( parentUUID, task.parentUUID()->c_str()) << "Checking Child's parent ";
		}
		else
		{
			FAIL( ) << "Unknown task in list";
		}
	}
	shared_ptr<ITimeAccessor> timeAccessor = db->getTimeAccessor();
	TimeList times = timeAccessor->getTimesChangedSince();
	ASSERT_EQ( 1, times.size()) << "Checking amount of times in database";
	for (auto item : times)
	{
		ASSERT_EQ( 1, item.ID()) << "TimeEntry ID";
		ASSERT_STREQ( "010c012c-00b9-40f6-80dd-018e011d0191", item.getUUID().c_str()) << "TimeEntry UUID";
		ASSERT_STREQ( "00b3015e-00d6-418e-81c8-0125012d0172", item.taskUUID()->c_str()) << "TimeEntry taskUUD";
		ASSERT_EQ( 1363597429, item.start()) << "TimeEntry start";
		ASSERT_EQ( 1363597541, item.stop()) << "TimeEntry stop";
		ASSERT_EQ( 1376388171, item.changed()) << "TimeEntry changed";
		ASSERT_EQ( false, item.deleted()) << "TimeEntry deleted";
	}

	syncManager.reset();
	while(syncManager.status() == SyncState::IDLE)
	{
		timer.on_signal_1_second();
	}
	while(syncManager.status() != SyncState::IDLE)
	{
		timer.on_signal_1_second();
	}

	tasks = taskAccessor->getTasksChangedSince();
	times = timeAccessor->getTimesChangedSince();
	ASSERT_EQ( 2, tasks->size()) << "Checking number of tasks after repeat syncing ";
	ASSERT_EQ( 1, times.size()) << "Checking number of times after repeat syncing ";
}


} /* namespace test */