
#include <string>
#include <libtimeit/sync/json_converter.h>
#include <libtimeit/db/task.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/utils.h>


namespace libtimeit
{
using namespace std;

string to_json(vector<task> tasks, string username)
{
	vector<json> items{};
	for (task node : tasks)
	{
		json item;
		item.set("name", json(node.name));
		item.set("id", json( static_cast<string>(node.id) ));
		if (node.parent_id.has_value())
		{
			json parent;
			parent.set( "id", json( static_cast<string>(node.parent_id.value()) ));
			item.set( "parent", parent);
		}
		item.set("completed", json(node.completed));
		item.set("lastChange", json(node.last_changed));
		item.set("deleted", json(node.deleted));

		item.set("idle", json((int64_t)node.idle));
		item.set("quiet", json(node.quiet));

		json owner;
		owner.set("username", json(username));
		item.set("owner", owner);

		items.push_back(item);
	}
	json result(items);
	return result.to_string();
}

string to_json(const Time_list& times)
{
	vector<json> items{};
	for (auto time : times)
	{
		json item;
		item.set("id", json( static_cast<string>(time.id)));
		item.set("task", json(static_cast<string>(time.owner_id)));
		item.set("start", json(time.start));
		item.set("stop", json(time.stop));
		item.set( "deleted",json(time.state == DELETED));
		item.set("changed",  json(time.changed));
		item.set("state", json((int64_t)time.state));
		item.set("comment", json(time.comment));
		json node;
		node.set("id", json(static_cast<string>(time.owner_id)));
		item.set("task", node);

		items.emplace_back(item);
	}
	json result(items);
	return result.to_string();
}

vector<task> to_tasks(const string &text)
{
	vector<task> return_value;

	json json_document=json::from_json_string(text);

	for (auto json_object: json_document.objects())
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
			optional<task_id> parent = {};
			auto parent_uuid = UUID::from_string(parent_string);
			if(parent_uuid.has_value())
			{
				parent = task_id(parent_uuid.value());
			}
			return_value.emplace_back(
					name,
					task_id(uuid.value()),
					completed,
					last_changed,
					parent,
					deleted,
					idle,
					quiet);
		}
	}
	return return_value;
}

Time_list to_times(const string &input)
{
	Time_list return_value;

	json json_document=json::from_json_string(input);

	for (json item: json_document.objects())
	{
		string uuid_string    = item.text("id");
		string task_id_string = item.by_name("task").text("id");
		time_t start          = item.integer("start");
		time_t stop           = item.integer("stop");
		time_t changed        = item.integer("changed");
		auto   state          = (Time_entry_state)item.integer("state");
		auto   deleted        = item.boolean( "deleted");
		auto   comment        = item.text("comment");
		if(deleted)
		{
			state = {DELETED};
		}

		auto uuid= UUID::from_string(uuid_string);
		auto owner= optional_task_id(task_id_string);
		if(uuid.has_value() && owner.has_value())
		{
			Time_entry time_item( time_id(*uuid), *owner, start, stop, state, changed, comment);
			return_value.push_back(time_item);
		}
	}
	return return_value;
}
}
