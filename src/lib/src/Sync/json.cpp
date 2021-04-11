
#include <string>
#include <jansson.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/db/task.h>
#include <libtimeit/exceptions/general_exception.h>

namespace libtimeit
{
using namespace std;

General_exception ge;

string to_json(vector<Task> tasks, string username)
{
	json_t *array = json_array();
	for (Task task : tasks)
	{
		json_t *obj = json_object();
		json_object_set(obj, "name", json_string(task.name.c_str()));
		json_object_set(obj, "id", json_string(task.uuid.c_str()));
		if (task.parent_uuid)
		{
			json_t *parent = json_object();
			json_object_set(parent, "id", json_string(task.parent_uuid->c_str()));
			json_object_set(obj, "parent", parent);
		}
		if (task.completed)
		{
			json_object_set(obj, "completed", json_true());
		}
		else
		{
			json_object_set(obj, "completed", json_false());
		}
		json_object_set(obj, "lastChange", json_integer(task.last_changed));
		if (task.deleted)
		{
			json_object_set(obj, "deleted", json_true());
		}
		else
		{
			json_object_set(obj, "deleted", json_false());
		}

		json_t *owner = json_object();
		json_object_set(owner, "username", json_string(username.c_str()));
		json_object_set(obj, "owner", owner);

		json_array_append(array, obj);
	}
	char *str = json_dumps(array, 0);
	if (str == nullptr)
	{
		//LCOV_EXCL_START
		ge.setMessage("Failed to create json string");
		throw ge;
		//LCOV_EXCL_STOP
	}
	string result = str;
	free(str);

	return result;
}
string to_json(const Time_list& times)
{
	json_t *array = json_array();
	for (auto time : times)
	{
		json_t *obj = json_object();
		json_object_set(obj, "id", json_string(time.uuid.c_str()));
		json_object_set(obj, "task", json_string(time.task_uuid->c_str()));
		json_object_set(obj, "start", json_integer(time.start));
		json_object_set(obj, "stop", json_integer(time.stop));
		if (time.deleted)
		{
			json_object_set(obj, "deleted", json_true());
		}
		else
		{
			json_object_set(obj, "deleted", json_false());
		}
		json_object_set(obj, "changed", json_integer(time.changed));

		json_t *task = json_object();
		json_object_set(task, "id", json_string(time.task_uuid->c_str()));
		json_object_set(obj, "task", task);

		json_array_append(array, obj);
	}
	char *str = json_dumps(array, 0);
	if (str == nullptr)
	{
		//LCOV_EXCL_START
		ge.setMessage("Failed to create json string");
		throw ge;
		//LCOV_EXCL_STOP
	}
	string result = str;
	free(str);

	return result;
}

vector<Task> to_tasks(const string &text)
{

	json_t *root;
	json_error_t error;

	vector<Task> retVal;

	root = json_loads(text.c_str(), 0, &error);

	for (unsigned int i = 0; i < json_array_size(root); i++)
	{
		string name;
		string uuidString;
		string parentString;
		bool completed;
		time_t lastChanged;
		bool deleted;

		json_t *object = json_array_get(root, i);
		json_t *j_name = json_object_get(object, "name");
		json_t *j_id = json_object_get(object, "id");
		json_t *j_parent = json_object_get(object, "parent");
		json_t *j_completed = json_object_get(object, "completed");
		json_t *j_lastChanged = json_object_get(object, "lastChange");
		json_t *j_deleted = json_object_get(object, "deleted");

		if (json_is_string(j_name))
		{
			name = json_string_value(j_name);
		}
		if (json_is_string(j_id))
		{
			uuidString = json_string_value(j_id);
		}
		if (json_is_object(j_parent))
		{
			json_t *j_parentID = json_object_get(j_parent, "id");
			if (json_is_string(j_parentID))
			{
				parentString = json_string_value(j_parentID);
			}
		}
		if (json_is_boolean(j_completed))
		{
			if (json_is_true(j_completed))
			{
				completed = true;
			}
			else
			{
				completed = false;
			}
		}
		if (json_is_integer(j_lastChanged))
		{
			lastChanged = json_integer_value(j_lastChanged);
		}
		if (json_is_boolean(j_deleted))
		{
			if (json_is_true(j_deleted))
			{
				deleted = true;
			}
			else
			{
				deleted = false;
			}
		}
		auto uuid= to_uuid(uuidString);
		if(uuid)
		{
			auto parent = to_uuid(parentString);
			Task task(name, 0, *uuid, completed, 0, lastChanged, parent, deleted);
			retVal.push_back(task);
		}
	}
	return retVal;
}

Time_list to_times(const string &input)
{

	json_t *root;
	json_error_t error;

	Time_list retVal;

	root = json_loads(input.c_str(), 0, &error);

	for (unsigned int i = 0; i < json_array_size(root); i++)
	{
		int64_t id = 0;
		string uuidString;
		string taskIDString;
		time_t start;
		time_t stop;
		time_t changed;
		bool deleted;

		json_t *object = json_array_get(root, i);
		json_t *j_task = json_object_get(object, "task");
		json_t *j_id = json_object_get(object, "id");
		json_t *j_start = json_object_get(object, "start");
		json_t *j_stop = json_object_get(object, "stop");
		json_t *j_changed = json_object_get(object, "changed");
		json_t *j_deleted = json_object_get(object, "deleted");

		if (json_is_object(j_task))
		{
			json_t *j_taskID = json_object_get(j_task, "id");
			if (json_is_string(j_taskID))
			{
				taskIDString = json_string_value(j_taskID);
			}
		}
		if (json_is_string(j_id))
		{
			uuidString = json_string_value(j_id);
		}
		if (json_is_integer(j_start))
		{
			start = json_integer_value(j_start);
		}
		if (json_is_integer(j_stop))
		{
			stop = json_integer_value(j_stop);
		}
		if (json_is_integer(j_changed))
		{
			changed = json_integer_value(j_changed);
		}
		if (json_is_boolean(j_deleted))
		{
			if (json_is_true(j_deleted))
			{
				deleted = true;
			}
			else
			{
				deleted = false;
			}
		}
		auto uuid= to_uuid(uuidString);
		auto taskID= to_uuid(taskIDString);
		if(uuid && taskID)
		{
			Time_entry item(id, *uuid, 0, *taskID, start, stop, deleted, STOPPED, changed);
			retVal.push_back(item);
		}
	}
	return retVal;
}
}
