#include <gtest/gtest.h>
#include "libtimeit/sync/json_converter.h"
#include "../Mock/mock_network.h"
#include "libtimeit/timer.h"
#include "libtimeit/sync/sync_manager.h"
#include "../DB/temp_db.h"

namespace test
{
using namespace std;
using namespace libtimeit;

TEST( SyncManager, fullSyncEmptyClient )
{
	notification_manager    notifier;
	temp_db      db (notifier);
	mock_network network;

	settings_accessor settings(db);
	settings.set_string("Username", "testman");
	settings.set_string("URL", "localhost");

	timer_base timer;
	sync_manager syncer( db, network, notifier, timer);

	std::string task_key = "/tasks/testman/0";
	std::string task_response = "[{\"name\": \"Child\", "
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
	std::string times_key = "/times/testman/0";

	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";

	std::string times_response = "[{\"id\": \"010c012c-00b9-40f6-80dd-018e011d0191\","
			"\"task\":{\"id\":\"00b3015e-00d6-418e-81c8-0125012d0172\"},"
			"\"start\": 1363597429, \"stop\": 1363597541, \"deleted\": false,"
			"\"changed\": 1376388171}]";
	network.set_response( task_key, task_response );
	network.set_response( times_key, times_response );


	while( syncer.status() == sync_state::idle)
	{
		timer.on_signal_1_second();
	}
	while( syncer.status() != sync_state::idle)
	{
		timer.on_signal_1_second();
	}

	task_accessor tasks( db);
	vector<task> result = tasks.changed_since();
	ASSERT_EQ( 2, result.size()) << "Checking amount of tasks in database";
	for (task item : result)
	{
		if ( item.name == "Parent")
		{
			ASSERT_EQ( system_clock::from_time_t(1375358076), item.last_changed) << "Checking Parent's change time";
		}
		else if ( item.name == "Child")
		{
			string parent_uuid = "013900e6-00dd-40f7-b0d6-00de00bf006b";
			ASSERT_EQ( system_clock::from_time_t(1375358093), item.last_changed) << "Checking Child's change time";
			ASSERT_EQ( parent_uuid, static_cast<string>(item.parent_id.value())) << "Checking Child's parent ";
		}
		else
		{
			FAIL( ) << "Unknown task in list";
		}
	}
	time_accessor times( db);
	time_list resulting_times = times.times_changed_since();
	ASSERT_EQ( 1, resulting_times.size()) << "Checking amount of times in database";
	for (auto item : resulting_times)
	{
		ASSERT_STREQ( "010c012c-00b9-40f6-80dd-018e011d0191", static_cast<string>(item.id).c_str()) << "TimeEntry UUID";
		ASSERT_STREQ( "00b3015e-00d6-418e-81c8-0125012d0172", static_cast<string>(item.owner_id).c_str()) << "TimeEntry taskUUD";
		ASSERT_EQ( system_clock::from_time_t( 1363597429 ), item.start) << "TimeEntry start";
		ASSERT_EQ( system_clock::from_time_t( 1363597541 )-system_clock::from_time_t( 1363597429 ) , item.duration) << "TimeEntry stop";
		ASSERT_EQ( system_clock::from_time_t( 1376388171 ), item.changed) << "TimeEntry changed";
		ASSERT_EQ( stopped, item.state) << "TimeEntry deleted";
	}

	syncer.reset();
	while( syncer.status() == sync_state::idle)
	{
		timer.on_signal_1_second();
	}
	while( syncer.status() != sync_state::idle)
	{
		timer.on_signal_1_second();
	}

	result = tasks.changed_since();
	resulting_times = times.times_changed_since();
	ASSERT_EQ( 2, result.size()) << "Checking number of tasks after repeat syncing ";
	ASSERT_EQ( 1, resulting_times.size()) << "Checking number of times after repeat syncing ";
}


} /* namespace test */
