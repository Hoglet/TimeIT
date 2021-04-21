#include <gtest/gtest.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/db/task.h>
#include <jansson.h>

using namespace libtimeit;

TEST( Json, simpleTaskTest )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\" : \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": false, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	vector<Task> tasks = to_tasks(json_string);

	std::string expectedName = "task1";
	std::string expectedParent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	std::string expectedUUID = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	int numberOfTasks = tasks.size();
	ASSERT_EQ( 1, numberOfTasks ) << "Number of tasks";

	if (numberOfTasks == 1)
	{
		Task task = tasks.at(0);
		bool expectedCompleted = false;
		bool expectedDeleted = false;
		time_t expectedChangeTime = 1374263745;
		ASSERT_EQ( expectedName, task.name) << "Name: ";
		ASSERT_EQ( expectedUUID, task.uuid.c_str()) << "UUID: ";
		ASSERT_EQ( expectedParent, task.parent_uuid->c_str()) << "Parent: ";
		ASSERT_EQ( expectedCompleted, task.completed) << "completed: ";
		ASSERT_EQ( expectedDeleted, task.deleted) << "Deleted: ";
		ASSERT_EQ( expectedChangeTime, task.last_changed) << "Expected change time: ";
	}
}

TEST( Json, simpleTaskTest2 )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": true, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	vector<Task> tasks = to_tasks(json_string);

	string expectedName = "task1";
	string expectedParent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	string expectedUUID = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	int numberOfTasks = tasks.size();
	ASSERT_EQ( 1, numberOfTasks) << "Number of tasks";

	if (numberOfTasks == 1)
	{
		Task task = tasks.at(0);
		bool expectedCompleted = true;
		bool expectedDeleted = true;
		time_t expectedChangeTime = 1374263745;
		ASSERT_EQ( expectedName, task.name) << "Name: ";
		ASSERT_EQ( expectedUUID, task.uuid.c_str()) << "UUID: ";
		ASSERT_EQ( expectedParent, task.parent_uuid->c_str()) << "Parent: ";
		ASSERT_EQ( expectedCompleted, task.completed) << "Completed: ";
		ASSERT_EQ( expectedDeleted, task.deleted) << "Deleted: ";
		ASSERT_EQ( expectedChangeTime, task.last_changed) << "Expected change time: ";
	}
}

TEST( Json, threeWayTaskTest )
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": false, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	vector<Task> tasks = to_tasks(json_string);
	std::string result = to_json(tasks, "tester");
	Task task1 = tasks.at(0);
	//Convert once more to be able to compare
	tasks = to_tasks(result);
	Task task2 = tasks.at(0);

	ASSERT_EQ( task1.name, task2.name) << "Name ";
	ASSERT_EQ(task1.uuid, task2.uuid) << "UUID: ";
	ASSERT_EQ(*task1.parent_uuid, *task2.parent_uuid) << "Parent: ";
	ASSERT_EQ( task1.completed, task2.completed) << "Completed: ";
	ASSERT_EQ( task1.deleted, task2.deleted) << "Deleted: ";
	ASSERT_EQ(task1.last_changed, task2.last_changed) << "Expected change time: ";

}

TEST( Json, threeWayTaskTest2 )
{
	string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": true, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	vector<Task> tasks = to_tasks(json_string);
	std::string result = to_json(tasks, "tester");
	Task task1 = tasks.at(0);
	//Convert once more to be able to compare
	tasks = to_tasks(result);
	Task task2 = tasks.at(0);

	ASSERT_EQ( task1.name, task2.name) << "Name ";
	ASSERT_EQ(task1.uuid, task2.uuid) << "UUID: ";
	ASSERT_EQ(*task1.parent_uuid, *task2.parent_uuid) << "Parent: ";
	ASSERT_EQ( task1.completed, task2.completed) << "Completed: ";
	ASSERT_EQ( task1.deleted, task2.deleted) << "Deleted: ";
	ASSERT_EQ(task1.last_changed, task2.last_changed) << "Expected change time: ";

}

TEST( Json, simpleTimeTest )
{
	string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	vector<Time_entry> times = to_times(json_string);

	string expectedTaskID = "00e1010f-00f2-40df-90b3-00f900ab009e";
	string expectedUUID = "01bd0176-00ed-4135-b181-014101790130";
	int numberOfItems = times.size();
	ASSERT_EQ( 1, numberOfItems) << "Number of items";

	if (numberOfItems == 1)
	{
		time_t expectedStart = 1363339855;
		time_t expectedStop = 1363342626;
		time_t expectedChange = 1376059170;
		Time_entry item = times.at(0);
		bool expectedDeleted = false;
		ASSERT_EQ( expectedUUID, item.uuid.c_str()) << "ID: ";
		ASSERT_EQ( expectedTaskID, item.task_uuid->c_str()) << "Task_ID: ";
		ASSERT_EQ( expectedDeleted, item.deleted) << "Deleted: ";
		ASSERT_EQ( expectedStart, item.start) << "Start: ";
		ASSERT_EQ( expectedStop, item.stop) << "Stop: ";
		ASSERT_EQ( expectedChange, item.changed) << "Expected change time: ";
	}
}

TEST( Json, simpleTimeTest2 )
{
	string json_string = R"(
[ {
  "id": "01bd0176-00ed-4135-b181-014101790130","task":{"id":"00e1010f-00f2-40df-90b3-00f900ab009e"},"start": 1363339855,"stop": 1363342626,"deleted": true,"changed": 1376059170, "owner":{"username":"tester"}}])";
	Time_list times = to_times(json_string);

	string expectedTaskID = "00e1010f-00f2-40df-90b3-00f900ab009e";
	string expectedUUID = "01bd0176-00ed-4135-b181-014101790130";
	int numberOfItems = times.size();
	ASSERT_EQ( 1, numberOfItems) << "Number of items";

	if (numberOfItems == 1)
	{
		time_t expectedStart = 1363339855;
		time_t expectedStop = 1363342626;
		time_t expectedChange = 1376059170;
		Time_entry item = times.at(0);
		bool expectedDeleted = true;
		ASSERT_EQ( expectedUUID, item.uuid.c_str()) << "ID: ";
		ASSERT_EQ( expectedTaskID, item.task_uuid->c_str()) << "Task_ID: ";
		ASSERT_EQ( expectedDeleted, item.deleted) << "Deleted: ";
		ASSERT_EQ( expectedStart, item.start) << "Start: ";
		ASSERT_EQ( expectedStop, item.stop) << "Stop: ";
		ASSERT_EQ( expectedChange, item.changed) << "Expected change time: ";
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

	vector<Time_entry> items = to_times(json_string);
	Time_entry item1 = items.at(0);
	//Convert once more to be able to compare
	std::string result = to_json(items);
	items = to_times(result);
	Time_entry item2 = items.at(0);

	ASSERT_EQ(item1.uuid, item2.uuid) << "ID ";
	ASSERT_EQ(item1.task_uuid, item2.task_uuid) << "Task_ID: ";
	ASSERT_EQ(item1.start, item2.start) << "Start: ";
	ASSERT_EQ(item1.stop, item2.stop) << "Stop: ";
	ASSERT_EQ(item1.deleted, item2.deleted) << "Deleted: ";
	ASSERT_EQ(item1.changed, item2.changed) << "Change time: ";

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
	vector<Time_entry> items = to_times(json_string);
	Time_entry item1 = items.at(0);
	//Convert once more to be able to compare
	std::string result = to_json(items);
	items = to_times(result);
	Time_entry item2 = items.at(0);

	ASSERT_EQ(item1.uuid, item2.uuid) << "ID ";
	ASSERT_EQ(item1.task_uuid, item2.task_uuid) << "Task_ID: ";
	ASSERT_EQ(item1.start, item2.start) << "Start: ";
	ASSERT_EQ(item1.stop, item2.stop) << "Stop: ";
	ASSERT_EQ(item1.deleted, item2.deleted) << "Deleted: ";
	ASSERT_EQ(item1.changed, item2.changed) << "Change time: ";

}

TEST( Json, testTaskStringGenerationTest )
{
	string name = "task1";
	auto parentID = UUID::from_string("71cf62ec-afc6-4a72-95a3-93a5b9f10b2d");
	auto uuid = UUID::from_string("73cf62ec-afc6-4a72-95a3-93a5b9f10b2d");
	time_t changeTime = 1374263745;

	Task task(name, 1, *uuid, false, 1, changeTime, parentID, false, 0, false);
	vector<Task> tasks;
	tasks.push_back(task);
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
	json_t *j_completed = json_object_get(object, "completed");
	json_t *j_lastChanged = json_object_get(object, "lastChange");
	json_t *j_deleted = json_object_get(object, "deleted");
	json_t *j_owner = json_object_get(object, "owner");

	ASSERT_EQ( name, json_string_value(j_name)) << "Name is incorrect";
	ASSERT_STREQ( uuid->c_str(), json_string_value(j_id)) << "id is incorrect";
	ASSERT_STREQ( parentID->c_str(), json_string_value(json_object_get(j_parent, "id"))) << "Parent id is incorrect";
	ASSERT_EQ( false, json_is_true(j_completed)) << "Completed is incorrect";
	ASSERT_EQ( changeTime, json_integer_value(j_lastChanged)) << "Last changed is incorrect";
	ASSERT_EQ( false, json_is_true(j_deleted)) << "Deleted is incorrect";
	ASSERT_STREQ( "tester", json_string_value(json_object_get(j_owner, "username"))) << "Owner id is incorrect";
}


