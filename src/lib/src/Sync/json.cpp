
#include <string>
#include <jansson.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/db/task.h>
#include <libtimeit/exceptions/general_exception.h>

namespace libtimeit
{
using namespace std;

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
		json_object_set(obj, "completed", json_boolean(task.completed));
		json_object_set(obj, "lastChange", json_integer(task.last_changed));
		json_object_set(obj, "deleted", json_boolean(task.deleted));

		json_object_set(obj, "idle", json_integer(task.idle));
		json_object_set(obj, "quiet", json_boolean(task.quiet));


		json_t *owner = json_object();
		json_object_set(owner, "username", json_string(username.c_str()));
		json_object_set(obj, "owner", owner);

		json_array_append(array, obj);
	}
	char *str = json_dumps(array, 0);    // NOLINT
	if (str == nullptr)
	{
		//LCOV_EXCL_START
		throw General_exception("Failed to create json string");
		//LCOV_EXCL_STOP
	}
	string result = str;
	free(str);     // NOLINT

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
		if (time.state == DELETED)
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
	char *str = json_dumps(array, 0);    // NOLINT
	if (str == nullptr)
	{
		//LCOV_EXCL_START
		throw General_exception("Failed to create json string");
		//LCOV_EXCL_STOP
	}
	string result = str;
	free(str);    // NOLINT

	return result;
}

vector<Task> to_tasks(const string &text)
{

	json_error_t error;

	vector<Task> return_value;

	json_t* root = json_loads(text.c_str(), 0, &error);

	for (unsigned int i = 0; i < json_array_size(root); i++)
	{
		string   name;
		string   uuid_string;
		string   parent_string;
		bool     completed   {false};
		time_t   last_changed {0};
		bool     deleted     {false};
		unsigned idle        {0};
		bool     quiet       {false};

		json_t *object         = json_array_get(root, i);
		json_t *j_name         = json_object_get(object, "name");
		json_t *j_id           = json_object_get(object, "id");
		json_t *j_parent       = json_object_get(object, "parent");
		json_t *j_completed    = json_object_get(object, "completed");
		json_t *j_last_changed = json_object_get(object, "lastChange");
		json_t *j_deleted      = json_object_get(object, "deleted");
		json_t *j_idle         = json_object_get(object, "idle");
		json_t *j_quiet        = json_object_get(object, "quiet");

		if (json_is_string(j_name))
		{
			name = json_string_value(j_name);
		}
		if (json_is_string(j_id))
		{
			uuid_string = json_string_value(j_id);
		}
		if (json_is_object(j_parent))
		{
			json_t *j_parent_id = json_object_get(j_parent, "id");
			if (json_is_string(j_parent_id))
			{
				parent_string = json_string_value(j_parent_id);
			}
		}
		if (json_is_boolean(j_completed))
		{
			completed = json_is_true(j_completed);
		}
		if (json_is_integer(j_last_changed))
		{
			last_changed = json_integer_value(j_last_changed);
		}
		if (json_is_boolean(j_deleted))
		{
			deleted = json_is_true(j_deleted);
		}
		if(json_is_integer(j_idle))
		{
			idle = (unsigned)json_integer_value(j_idle);
		}
		if(json_is_boolean(j_quiet))
		{
			quiet = json_boolean_value(j_quiet);
		}
		auto uuid= UUID::from_string(uuid_string);
		if(uuid)
		{
			auto parent = UUID::from_string(parent_string);
			Task task(name, 0, *uuid, completed, 0, last_changed, parent, deleted, idle, quiet);
			return_value.push_back(task);
		}
	}
	return return_value;
}

Time_list to_times(const string &input)
{

	json_error_t error{};

	Time_list return_value;

	json_t* root = json_loads(input.c_str(), 0, &error);

	for (unsigned int i = 0; i < json_array_size(root); i++)
	{
		int64_t id = 0;
		string uuid_string;
		string task_id_string;
		time_t start   {0};
		time_t stop    {0};
		time_t changed {0};
		auto   state   {STOPPED};

		json_t *object = json_array_get(root, i);
		json_t *j_task = json_object_get(object, "task");
		json_t *j_id = json_object_get(object, "id");
		json_t *j_start = json_object_get(object, "start");
		json_t *j_stop = json_object_get(object, "stop");
		json_t *j_changed = json_object_get(object, "changed");
		json_t *j_deleted = json_object_get(object, "deleted");

		if (json_is_object(j_task))
		{
			json_t *j_task_id = json_object_get(j_task, "id");
			if (json_is_string(j_task_id))
			{
				task_id_string = json_string_value(j_task_id);
			}
		}
		if (json_is_string(j_id))
		{
			uuid_string = json_string_value(j_id);
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
				state = DELETED;
			}
		}
		auto uuid= UUID::from_string(uuid_string);
		auto task_id= UUID::from_string(task_id_string);
		if(uuid && task_id)
		{
			Time_entry item(id, *uuid, 0, *task_id, start, stop, state, changed);
			return_value.push_back(item);
		}
	}
	return return_value;
}
}
