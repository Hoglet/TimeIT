#include "SyncTest.h"
#include "Network.h"
#include "cute.h"
#include "cute_runner.h"
#include "Json.h"
#include "Mock/MockDatabase.h"
#include "Mock/MockNetwork.h"
#include "Timer.h"
#include "SyncManager.h"
#include "DB/TempDB.h"

namespace Test
{
using namespace std;

void SyncManager_fullSyncEmptyClient()
{
	shared_ptr<IDatabase> db = shared_ptr<IDatabase>(new TempDB());
	shared_ptr<MockNetwork> mockNetwork = shared_ptr<MockNetwork>(new MockNetwork());
	shared_ptr<INetwork> network = static_pointer_cast<INetwork>(mockNetwork);
	shared_ptr<Timer> timer = shared_ptr<Timer>(new Timer());
	SyncManager syncManager(db, timer, network);

	std::string taskKey = "/tasks/testman/";
	std::string taskResponse = "[{\"name\": \"Child\", "
			"\"id\": \"00b3015e-00d6-418e-81c8-0125012d0172\", "
			"\"parent\": \"013900e6-00dd-40f7-b0d6-00de00bf006b\", "
			"\"lastChange\": 1375358093, "
			"\"completed\": false,"
			"\"deleted\": true},"
			"{\"name\": \"Parent\", "
			"\"id\": \"013900e6-00dd-40f7-b0d6-00de00bf006b\", "
			"\"lastChange\": 1375358076, "
			"\"deleted\": false, "
			"\"completed\": false}]";
	std::string timesKey = "/times/testman/";
	std::string timesResponse = "[{\"id\": \"010c012c-00b9-40f6-80dd-018e011d0191\","
			"\"task\": \"00b3015e-00d6-418e-81c8-0125012d0172\","
			"\"start\": 1363597429, \"stop\": 1363597541, \"deleted\": false,"
			"\"changed\": 1376388171}]";
	mockNetwork->setResponse( taskKey, taskResponse);
	mockNetwork->setResponse( timesKey, timesResponse);

	syncManager.on_signal_10_seconds();

	shared_ptr<ITaskAccessor> taskAccessor = db->getTaskAccessor();
	shared_ptr<vector<Task> > tasks = taskAccessor->getTasksChangedSince();
	ASSERT_EQUALM("Checking amount of tasks in database", 2, tasks->size());
	for ( Task task : *tasks)
	{
		if(task.getName() == "Parent")
		{
			ASSERT_EQUALM("Checking Parent's change time", 1375358076, task.getLastChanged());
		}
		else if(task.getName() == "Child")
		{
			string parentUUID = "013900e6-00dd-40f7-b0d6-00de00bf006b";
			ASSERT_EQUALM("Checking Child's change time", 1375358093, task.getLastChanged());
			ASSERT_EQUALM("Checking Child's parent ", parentUUID, task.getParentUUID());
		}
		else
		{
			ASSERTM("Unkown task in list", false);
		}
	}
	shared_ptr<ITimeAccessor> timeAccessor = db->getTimeAccessor();
	shared_ptr<vector<TimeEntry> > times = timeAccessor->getTimesChangedSince();
	ASSERT_EQUALM("Checking amount of times in database", 1, times->size());
	for ( TimeEntry item : *times)
	{
		ASSERT_EQUALM("TimeEntry ID", 1, item.getID());
		ASSERT_EQUALM("TimeEntry UUID", string("010c012c-00b9-40f6-80dd-018e011d0191"), item.getUUID());
		ASSERT_EQUALM("TimeEntry taskUUD", string("00b3015e-00d6-418e-81c8-0125012d0172"), item.getTaskUUID());
		ASSERT_EQUALM("TimeEntry start", 1363597429, item.getStart());
		ASSERT_EQUALM("TimeEntry stop", 1363597541, item.getStop());
		ASSERT_EQUALM("TimeEntry changed", 1376388171, item.getLastChanged());
		ASSERT_EQUALM("TimeEntry deleted", false, item.getDeleted());
	}


	syncManager.completeSync();

	tasks = taskAccessor->getTasksChangedSince();
	times = timeAccessor->getTimesChangedSince();
	ASSERT_EQUALM("Checking number of tasks after repeat syncing ", 2, tasks->size());
	ASSERT_EQUALM("Checking number of times after repeat syncing ", 1, times->size());
}

cute::suite make_suite_SyncTest()
{
	cute::suite s;
	s.push_back(CUTE(SyncManager_fullSyncEmptyClient));
	return s;
}

} /* namespace Test */
