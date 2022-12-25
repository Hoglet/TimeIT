#include <gtest/gtest.h>
#include <libtimeit/sync/json_converter.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/utils.h>

using namespace libtimeit;

TEST( Json, simpleTaskTest )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\" : \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	vector<task> tasks = to_tasks(json_string);

	std::string expected_name = "task1";
	std::string expected_parent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	std::string expected_uuid = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	int number_of_tasks = tasks.size();
	ASSERT_EQ( 1, number_of_tasks ) << "Number of tasks";

	if ( number_of_tasks == 1)
	{
		task task_1 = tasks.at( 0);
		bool expected_deleted = false;
		auto expected_change_time = system_clock::from_time_t( 1374263745);
		ASSERT_EQ( expected_name, task_1.name) << "Name: ";
		ASSERT_EQ( expected_uuid, static_cast<string>(task_1.id)) << "UUID: ";
		ASSERT_EQ( expected_parent, static_cast<string>(task_1.parent_id.value())) << "Parent: ";
		ASSERT_EQ( expected_deleted, task_1.deleted) << "Deleted: ";
		ASSERT_EQ( expected_change_time, task_1.last_changed) << "Expected change time: ";
	}
}

TEST( Json, simpleTaskTest2 )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	vector<task> tasks = to_tasks(json_string);

	string expected_name = "task1";
	string expected_parent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	string expected_uuid = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	int number_of_tasks = tasks.size();
	ASSERT_EQ( 1, number_of_tasks) << "Number of tasks";

	if ( number_of_tasks == 1)
	{
		task task_1 = tasks.at( 0);
		bool expected_deleted = true;
		auto expected_change_time = system_clock::from_time_t( 1374263745);
		ASSERT_EQ( expected_name, task_1.name) << "Name: ";
		ASSERT_EQ( expected_uuid, static_cast<string>(task_1.id)) << "UUID: ";
		ASSERT_EQ( expected_parent, static_cast<string>(task_1.parent_id.value())) << "Parent: ";
		ASSERT_EQ( expected_deleted, task_1.deleted) << "Deleted: ";
		ASSERT_EQ( expected_change_time, task_1.last_changed) << "Expected change time: ";
	}
}

TEST( Json, threeWayTaskTest )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	vector<task> tasks = to_tasks(json_string);
	std::string result = to_json(tasks, "tester");
	task task_1 = tasks.at( 0);
	//Convert once more to be able to compare
	tasks = to_tasks(result);
	task task_2 = tasks.at( 0);

	ASSERT_EQ( task_1.name, task_2.name) << "Name ";
	ASSERT_EQ( task_1.id, task_2.id) << "UUID: ";
	ASSERT_EQ( *task_1.parent_id, *task_2.parent_id) << "Parent: ";
	ASSERT_EQ( task_1.deleted, task_2.deleted) << "Deleted: ";
	ASSERT_EQ( task_1.last_changed, task_2.last_changed) << "Expected change time: ";

}

TEST( Json, threeWayTaskTest2 )
{
	string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	vector<task> tasks = to_tasks(json_string);
	std::string result = to_json(tasks, "tester");
	task task_1 = tasks.at( 0);
	//Convert once more to be able to compare
	tasks = to_tasks(result);
	task task_2 = tasks.at( 0);

	ASSERT_EQ( task_1.name, task_2.name) << "Name ";
	ASSERT_EQ( task_1.id, task_2.id) << "UUID: ";
	ASSERT_EQ( *task_1.parent_id, *task_2.parent_id) << "Parent: ";
	ASSERT_EQ( task_1.deleted, task_2.deleted) << "Deleted: ";
	ASSERT_EQ( task_1.last_changed, task_2.last_changed) << "Expected change time: ";

}

TEST( Json, simpleTimeTest )
{
	string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	vector<time_entry> times = to_times( json_string);

	string expected_task_id = "00e1010f-00f2-40df-90b3-00f900ab009e";
	string expected_uuid = "01bd0176-00ed-4135-b181-014101790130";
	int number_of_items = times.size();
	ASSERT_EQ( 1, number_of_items) << "Number of items";

	if ( number_of_items == 1)
	{
		auto expected_start  = system_clock::from_time_t( 1363339855 );
		auto expected_stop   = system_clock::from_time_t( 1363342626 );
		auto expected_change = system_clock::from_time_t( 1376059170 );
		time_entry item = times.at( 0);
		bool expected_state = stopped;
		ASSERT_EQ( expected_uuid, static_cast<string>(item.id)) << "id: ";
		ASSERT_EQ( expected_task_id, static_cast<string>(item.owner_id)) << "Task_ID: ";
		ASSERT_EQ( expected_state, item.state) << "State: ";
		ASSERT_EQ( expected_start, item.start) << "Start: ";
		ASSERT_EQ( expected_stop, item.stop) << "Stop: ";
		ASSERT_EQ( expected_change, item.changed) << "Expected change time: ";
	}
}

TEST( Json, simpleTimeTest2 )
{
	string json_string = R"(
[ {
  "id": "01bd0176-00ed-4135-b181-014101790130","task":{"id":"00e1010f-00f2-40df-90b3-00f900ab009e"},"start": 1363339855,"stop": 1363342626,"deleted": true,"changed": 1376059170, "owner":{"username":"tester"}}])";
	time_list times = to_times( json_string);

	string expected_task_id = "00e1010f-00f2-40df-90b3-00f900ab009e";
	string expected_uuid = "01bd0176-00ed-4135-b181-014101790130";
	int number_of_items = times.size();
	ASSERT_EQ( 1, number_of_items) << "Number of items";

	if ( number_of_items == 1)
	{
		auto expected_start  = system_clock::from_time_t( 1363339855 );
		auto expected_stop   = system_clock::from_time_t( 1363342626 );
		auto expected_change = system_clock::from_time_t( 1376059170 );
		time_entry item = times.at( 0);
		auto expected_state = deleted;
		ASSERT_EQ( expected_uuid, static_cast<string>(item.id)) << "id: ";
		ASSERT_EQ( expected_task_id, static_cast<string>(item.owner_id)) << "Task_ID: ";
		ASSERT_EQ( expected_state, item.state) << "State: ";
		ASSERT_EQ( expected_start, item.start) << "Start: ";
		ASSERT_EQ( expected_stop, item.stop) << "Stop: ";
		ASSERT_EQ( expected_change, item.changed) << "Expected change time: ";
	}
}

TEST( Json, threeWayTimeTest )
{
	string json_string =
			R"(
[{
	"id": "01bd0176-00ed-4135-b181-014101790130",
    "task":
	{
		"id" : "00e1010f-00f2-40df-90b3-00f900ab009e"
	},
	"start": 1363339855,
	"stop": 1363342626,
	"deleted": false,
	"changed": 1376059170,
	"owner":{"username":"tester"}
}])";

	vector<time_entry> items = to_times( json_string);
	time_entry item_1 = items.at( 0);
	//Convert once more to be able to compare
	std::string result = to_json(items);
	items = to_times(result);
	time_entry item_2 = items.at( 0);

	ASSERT_EQ( item_1.id, item_2.id) << "id ";
	ASSERT_EQ( item_1.owner_id, item_2.owner_id) << "Task_ID: ";
	ASSERT_EQ( item_1.start, item_2.start) << "Start: ";
	ASSERT_EQ( item_1.stop, item_2.stop) << "Stop: ";
	ASSERT_EQ( item_1.state, item_2.state) << "Deleted: ";
	ASSERT_EQ( item_1.changed, item_2.changed) << "Change time: ";

}

TEST( Json, threeWayTimeTest2 )
{
	string json_string =
			R"(
[{
	"id": "01bd0176-00ed-4135-b181-014101790130",
    "task":
	{
		"id" : "00e1010f-00f2-40df-90b3-00f900ab009e"
	},	"taskID": "00e1010f-00f2-40df-90b3-00f900ab009e",
	"start": 1363339855,
	"stop": 1363342626,
	"deleted": true,
	"changed": 1376059170,"owner": {"username":"tester"}
}])";
	vector<time_entry> items = to_times( json_string);
	time_entry item_1 = items.at( 0);
	//Convert once more to be able to compare
	std::string result = to_json(items);
	items = to_times(result);
	time_entry item_2 = items.at( 0);

	ASSERT_EQ( item_1.id, item_2.id) << "id ";
	ASSERT_EQ( item_1.owner_id, item_2.owner_id) << "Task_ID: ";
	ASSERT_EQ( item_1.start, item_2.start) << "Start: ";
	ASSERT_EQ( item_1.stop, item_2.stop) << "Stop: ";
	ASSERT_EQ( item_1.state, item_2.state) << "Deleted: ";
	ASSERT_EQ( item_1.changed, item_2.changed) << "Change time: ";

}

TEST( Json, testTaskStringGenerationTest )
{
	string name = "task1";
	auto parent_id = optional_task_id( "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d");
	auto id = uuid::from_string( "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d");
	auto change_time = system_clock::from_time_t( 1374263745);

	task task_1( name, task_id( *id), change_time, parent_id, false, 0min, false);
	vector<task> tasks;
	tasks.push_back( task_1);
	string result = to_json(tasks, "tester");

	json_t *root;
	json_error_t error;

	root = json_loads(result.c_str(), 0, &error);

	ASSERT_EQ( 1, json_array_size(root)) << "Number if tasks are wrong ";

	json_t *object = json_array_get(root, 0);

	ASSERT_EQ( 9, json_object_size(object)) << "Number of fields are wrong ";

	json_t *j_name = json_object_get(object, "name");
	json_t *j_id = json_object_get(object, "id");
	json_t *j_parent = json_object_get(object, "parent");
	json_t *j_last_changed = json_object_get( object, "lastChange");
	json_t *j_deleted = json_object_get(object, "deleted");
	json_t *j_owner = json_object_get(object, "owner");

	ASSERT_EQ( name, json_string_value(j_name)) << "Name is incorrect";
	ASSERT_STREQ( id->c_str(), json_string_value( j_id)) << "id is incorrect";
	ASSERT_EQ( static_cast<string>(parent_id.value()), json_string_value( json_object_get( j_parent, "id"))) << "Parent id is incorrect";
	ASSERT_EQ( change_time, system_clock::from_time_t( json_integer_value( j_last_changed))) << "Last changed is incorrect";
	ASSERT_EQ( false, json_is_true(j_deleted)) << "Deleted is incorrect";
	ASSERT_STREQ( "tester", json_string_value(json_object_get(j_owner, "username"))) << "Owner id is incorrect";
}
