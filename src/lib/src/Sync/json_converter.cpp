
#include <string>
#include <libtimeit/sync/json_converter.h>
#include <libtimeit/db/task.h>
#include <libtimeit/sync/json.h>

namespace libtimeit
{
using namespace std;

string to_json(vector<Task> tasks, string username)
{
	vector<Json> items{};
	for (Task task : tasks)
	{
		Json item;
		item.set("name", Json(task.name));
		item.set("id", Json(task.uuid.to_string()));
		if (task.parent_uuid)
		{
			Json parent;
			parent.set( "id", Json(task.parent_uuid->to_string()));
			item.set( "parent", parent);
		}
		item.set("completed", Json(task.completed));
		item.set("lastChange", Json(task.last_changed));
		item.set("deleted", Json(task.deleted));

		item.set("idle", Json((int64_t)task.idle));
		item.set("quiet", Json(task.quiet));

		Json owner;
		owner.set("username", Json(username));
		item.set("owner", owner);

		items.emplace_back(item);
	}
	Json result(items);
	return result.to_string();
}

string to_json(const Time_list& times)
{
	vector<Json> items{};
	for (auto time : times)
	{
		Json item;
		item.set("id", Json( time.uuid.to_string()));
		item.set("task", Json(time.task_uuid->to_string()));
		item.set("start", Json(time.start));
		item.set("stop", Json(time.stop));
		item.set( "deleted",Json(time.state == DELETED));
		item.set("changed",  Json(time.changed));
		item.set("state", Json((int64_t)time.state));

		Json task;
		task.set("id", Json(time.task_uuid->to_string()));
		item.set( "task", task);

		items.emplace_back(item);
	}
	Json result(items);
	return result.to_string();
}

vector<Task> to_tasks(const string &text)
{
	vector<Task> return_value;

	Json json=Json::from_json_string(text);

	for (auto json_object: json.objects())
	{
		string   name            = json_object.text("name");
		string   uuid_string     = json_object.text("id");
		string   parent_string   = json_object.by_name("parent").text("id");
		bool     completed       = json_object.boolean("completed");
		time_t   last_changed    = json_object.integer("lastChange");
		bool     deleted         = json_object.boolean("deleted");
		auto     idle            = (unsigned)json_object.integer("idle");
		bool     quiet           = json_object.boolean("quiet");
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
	Time_list return_value;

	Json json=Json::from_json_string(input);

	for (Json item: json.objects())
	{
		int64_t id            = 0;
		string uuid_string    = item.text("id");
		string task_id_string = item.by_name("task").text("id");
		time_t start          = item.integer("start");
		time_t stop           = item.integer("stop");;
		time_t changed        = item.integer("changed");
		auto   state          = (Time_entry_state)item.integer("state");
		auto   deleted        = item.boolean( "deleted");
		if(deleted)
		{
			state = {DELETED};
		}

		auto uuid= UUID::from_string(uuid_string);
		auto task_id= UUID::from_string(task_id_string);
		if(uuid.has_value() && task_id.has_value())
		{
			Time_entry time_item(id, *uuid, 0, *task_id, start, stop, state, changed);
			return_value.push_back(time_item);
		}
	}
	return return_value;
}
}
