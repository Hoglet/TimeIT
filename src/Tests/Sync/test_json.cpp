#include "test_json.h"
#include "cute.h"
#include "Json.h"
#include "Task.h"
#include "cute_runner.h"
#include <jansson.h>

using namespace DB;

namespace Test
{

void Json_simpleTaskTest()
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\" : \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": false, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<Task>> tasks = Json::toTasks(json_string);

	std::string expectedName = "task1";
	std::string expectedParent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	std::string expectedUUID = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	time_t expectedChangeTime = 1374263745;
	int numberOfTasks = tasks->size();
	ASSERT_EQUALM("Number of tasks", 1, numberOfTasks);

	if (numberOfTasks == 1)
	{
		Task task = tasks->at(0);
		bool expectedCompleted = false;
		bool expectedDeleted = false;
		ASSERT_EQUALM("Name: ", expectedName, task.getName());
		ASSERT_EQUALM("UUID: ", expectedUUID, task.getUUID());
		ASSERT_EQUALM("Parent: ", expectedParent, task.getParentUUID());
		ASSERT_EQUALM("completed: ", expectedCompleted, task.getCompleted());
		ASSERT_EQUALM("Deleted: ", expectedDeleted, task.getDeleted());
		ASSERT_EQUALM("Expected change time: ", expectedChangeTime, task.getLastChanged());
	}
}

void Json_simpleTaskTest2()
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": true, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<Task>> tasks = Json::toTasks(json_string);

	std::string expectedName = "task1";
	std::string expectedParent = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	std::string expectedUUID = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	time_t expectedChangeTime = 1374263745;
	int numberOfTasks = tasks->size();
	ASSERT_EQUALM("Number of tasks", 1, numberOfTasks);

	if (numberOfTasks == 1)
	{
		Task task = tasks->at(0);
		bool expectedCompleted = true;
		bool expectedDeleted = true;
		ASSERT_EQUALM("Name: ", expectedName, task.getName());
		ASSERT_EQUALM("UUID: ", expectedUUID, task.getUUID());
		ASSERT_EQUALM("Parent: ", expectedParent, task.getParentUUID());
		ASSERT_EQUALM("Completed: ", expectedCompleted, task.getCompleted());
		ASSERT_EQUALM("Deleted: ", expectedDeleted, task.getDeleted());
		ASSERT_EQUALM("Expected change time: ", expectedChangeTime, task.getLastChanged());
	}
}

void Json_threeWayTaskTest()
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": false, \"deleted\": false, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	std::shared_ptr<std::vector<Task>> tasks = Json::toTasks(json_string);
	std::string result = Json::toJson(tasks, "tester");
	Task task1 = tasks->at(0);
	//Convert once more to be able to compare
	tasks = Json::toTasks(result);
	Task task2 = tasks->at(0);

	ASSERT_EQUALM("Name ", task1.getName(), task2.getName());
	ASSERT_EQUALM("UUID: ", task1.getUUID(), task2.getUUID());
	ASSERT_EQUALM("Parent: ", task1.getParentUUID(), task2.getParentUUID());
	ASSERT_EQUALM("Completed: ", task1.getCompleted(), task2.getCompleted());
	ASSERT_EQUALM("Deleted: ", task1.getDeleted(), task2.getDeleted());
	ASSERT_EQUALM("Expected change time: ", task1.getLastChanged(), task2.getLastChanged());

}
void Json_threeWayTaskTest2()
{
	std::string json_string =
			"[{\"name\": \"task1\", \"id\": \"73cf62ec-afc6-4a72-95a3-93a5b9f10b2d\", "
					"\"parent\":{\"id\": \"71cf62ec-afc6-4a72-95a3-93a5b9f10b2d\"}, \"completed\": true, \"deleted\": true, \"lastChange\": 1374263745, \"owner\":{\"user\":\"tester\"}}]";
	std::shared_ptr<std::vector<Task>> tasks = Json::toTasks(json_string);
	std::string result = Json::toJson(tasks, "tester");
	Task task1 = tasks->at(0);
	//Convert once more to be able to compare
	tasks = Json::toTasks(result);
	Task task2 = tasks->at(0);

	ASSERT_EQUALM("Name ", task1.getName(), task2.getName());
	ASSERT_EQUALM("UUID: ", task1.getUUID(), task2.getUUID());
	ASSERT_EQUALM("Parent: ", task1.getParentUUID(), task2.getParentUUID());
	ASSERT_EQUALM("Completed: ", task1.getCompleted(), task2.getCompleted());
	ASSERT_EQUALM("Deleted: ", task1.getDeleted(), task2.getDeleted());
	ASSERT_EQUALM("Expected change time: ", task1.getLastChanged(), task2.getLastChanged());

}

void Json_simpleTimeTest()
{
	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<DB::TimeEntry>> times = Json::toTimes(json_string);

	std::string expectedTaskID = "00e1010f-00f2-40df-90b3-00f900ab009e";
	std::string expectedUUID = "01bd0176-00ed-4135-b181-014101790130";
	time_t expectedStart = 1363339855;
	time_t expectedStop = 1363342626;
	time_t expectedChange = 1376059170;
	int numberOfItems = times->size();
	ASSERT_EQUALM("Number of items", 1, numberOfItems);

	if (numberOfItems == 1)
	{
		TimeEntry item = times->at(0);
		bool expectedDeleted = false;
		ASSERT_EQUALM("ID: ", expectedUUID, item.getUUID());
		ASSERT_EQUALM("TaskID: ", expectedTaskID, item.getTaskUUID());
		ASSERT_EQUALM("Deleted: ", expectedDeleted, item.getDeleted());
		ASSERT_EQUALM("Start: ", expectedStart, item.getStart());
		ASSERT_EQUALM("Stop: ", expectedStop, item.getStop());
		ASSERT_EQUALM("Expected change time: ", expectedChange, item.getLastChanged());
	}
}
void Json_simpleTimeTest2()
{
	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"task\":{\"id\":\"00e1010f-00f2-40df-90b3-00f900ab009e\"},\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": true,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<DB::TimeEntry>> times = Json::toTimes(json_string);

	std::string expectedTaskID = "00e1010f-00f2-40df-90b3-00f900ab009e";
	std::string expectedUUID = "01bd0176-00ed-4135-b181-014101790130";
	time_t expectedStart = 1363339855;
	time_t expectedStop = 1363342626;
	time_t expectedChange = 1376059170;
	int numberOfItems = times->size();
	ASSERT_EQUALM("Number of items", 1, numberOfItems);

	if (numberOfItems == 1)
	{
		TimeEntry item = times->at(0);
		bool expectedDeleted = true;
		ASSERT_EQUALM("ID: ", expectedUUID, item.getUUID());
		ASSERT_EQUALM("TaskID: ", expectedTaskID, item.getTaskUUID());
		ASSERT_EQUALM("Deleted: ", expectedDeleted, item.getDeleted());
		ASSERT_EQUALM("Start: ", expectedStart, item.getStart());
		ASSERT_EQUALM("Stop: ", expectedStop, item.getStop());
		ASSERT_EQUALM("Expected change time: ", expectedChange, item.getLastChanged());
	}
}

void Json_threeWayTimeTest()
{
	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"taskID\": \"00e1010f-00f2-40df-90b3-00f900ab009e\",\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": false,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<DB::TimeEntry>> items = Json::toTimes(json_string);
	TimeEntry item1 = items->at(0);
	//Convert once more to be able to compare
	std::string result = Json::toJson(items);
	items = Json::toTimes(result);
	TimeEntry item2 = items->at(0);

	ASSERT_EQUALM("ID ", item1.getUUID(), item2.getUUID());
	ASSERT_EQUALM("TaskID: ", item1.getTaskUUID(), item2.getTaskUUID());
	ASSERT_EQUALM("Start: ", item1.getStart(), item2.getStart());
	ASSERT_EQUALM("Stop: ", item1.getStop(), item2.getStop());
	ASSERT_EQUALM("Deleted: ", item1.getDeleted(), item2.getDeleted());
	ASSERT_EQUALM("Change time: ", item1.getLastChanged(), item2.getLastChanged());

}
void Json_threeWayTimeTest2()
{
	std::string json_string =
			"[ {\"id\": \"01bd0176-00ed-4135-b181-014101790130\",\"taskID\": \"00e1010f-00f2-40df-90b3-00f900ab009e\",\"start\": 1363339855,\"stop\": 1363342626,\"deleted\": true,\"changed\": 1376059170, \"owner\":{\"username\":\"tester\"}}]";
	std::shared_ptr<std::vector<DB::TimeEntry>> items = Json::toTimes(json_string);
	TimeEntry item1 = items->at(0);
	//Convert once more to be able to compare
	std::string result = Json::toJson(items);
	items = Json::toTimes(result);
	TimeEntry item2 = items->at(0);

	ASSERT_EQUALM("ID ", item1.getUUID(), item2.getUUID());
	ASSERT_EQUALM("TaskID: ", item1.getTaskUUID(), item2.getTaskUUID());
	ASSERT_EQUALM("Start: ", item1.getStart(), item2.getStart());
	ASSERT_EQUALM("Stop: ", item1.getStop(), item2.getStop());
	ASSERT_EQUALM("Deleted: ", item1.getDeleted(), item2.getDeleted());
	ASSERT_EQUALM("Change time: ", item1.getLastChanged(), item2.getLastChanged());

}

void Json_testTaskStringGenerationTest()
{
	std::string name = "task1";
	std::string parentID = "71cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	std::string UUID = "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d";
	time_t changeTime = 1374263745;

	Task task(name, 1, UUID, false, 1, changeTime, parentID, false);
	std::shared_ptr<std::vector<Task>> tasks = std::shared_ptr<std::vector<Task>>(new std::vector<Task>);
	tasks->push_back(task);
	std::string result = Json::toJson(tasks, "tester");

	json_t *root;
	json_error_t error;

	root = json_loads(result.c_str(), 0, &error);

	ASSERT_EQUALM("Number if tasks are wrong ", 1, json_array_size(root));

	json_t *object = json_array_get(root, 0);

	ASSERT_EQUALM("Number of fields are wrong ", 7, json_object_size(object));

	json_t *j_name = json_object_get(object, "name");
	json_t *j_id = json_object_get(object, "id");
	json_t *j_parent = json_object_get(object, "parent");
	json_t *j_completed = json_object_get(object, "completed");
	json_t *j_lastChanged = json_object_get(object, "lastChange");
	json_t *j_deleted = json_object_get(object, "deleted");
	json_t *j_owner = json_object_get(object, "owner");

	ASSERT_EQUALM("Name is incorrect", name, json_string_value(j_name));
	ASSERT_EQUALM("id is incorrect", UUID, json_string_value(j_id));
	ASSERT_EQUALM("Parent id is incorrect", parentID, json_string_value(json_object_get(j_parent, "id")));
	ASSERT_EQUALM("Completed is incorrect", false, json_is_true(j_completed));
	ASSERT_EQUALM("Last changed is incorrect", changeTime, json_integer_value(j_lastChanged));
	ASSERT_EQUALM("Deleted is incorrect", false, json_is_true(j_deleted));
	ASSERT_EQUALM("Owner id is incorrect", "tester", json_string_value(json_object_get(j_owner, "username")));

}

cute::suite make_suite_JsonTest()
{
	cute::suite s;
	s.push_back(CUTE(Json_simpleTaskTest));
	s.push_back(CUTE(Json_simpleTaskTest2));
	s.push_back(CUTE(Json_threeWayTaskTest));
	s.push_back(CUTE(Json_simpleTimeTest));
	s.push_back(CUTE(Json_threeWayTimeTest));
	s.push_back(CUTE(Json_simpleTimeTest2));
	s.push_back(CUTE(Json_threeWayTaskTest2));
	s.push_back(CUTE(Json_threeWayTimeTest2));
	s.push_back(CUTE(Json_testTaskStringGenerationTest));
	return s;
}

} /* namespace Test */
