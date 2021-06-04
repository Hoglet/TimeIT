#include <libtimeit/sync/sync_manager.h>

#include <libtimeit/utils.h>
#include <libtimeit/sync/json_converter.h>
#include <libtimeit/db/default_values.h>
#include <sstream>
#include <libintl.h>
#include <fmt/core.h>
#include <libtimeit/internal/gettext.h>
#include <libtimeit/logging.h>

namespace libtimeit
{

static const int HTTP_OK = 200;
static const int HTTP_UNAUTHORIZED = 401;
static const int MINUTE = 60;

using namespace std;

const int ONE_DAY = 60 * 60 * 24;



Sync_manager::Sync_manager(
		Database &database,
		INetwork  &op_network,
		Notifier& notifier_,
		Timer&    timer
		):
		Timer_observer(timer),
		task_accessor(database),
		time_accessor(database),
		settings_accessor(database),
		network(op_network),
		notifier(notifier_)
{
}


Sync_state Sync_manager::status()
{
	return state;
}

void Sync_manager::on_signal_1_second()
{
	switch (state)
	{
		case Sync_state::IDLE:
		{
			time_t now = libtimeit::now();
			if (is_active() && now > next_full_sync)
			{
				next_full_sync = now + ONE_DAY;
				last_sync = 0;
			}
			if (is_active() && now > next_sync)
			{
				logg("Starting sync run");
				state = Sync_state::TASK_REQUEST;
				current_sync = now;
				next_sync = get_next_sync(current_sync);
			}
		}
			break;
		case Sync_state::TASK_REQUEST:
			logg("Requesting tasks");
			outstanding_request = request_tasks(last_sync);
			state = Sync_state::WAIT;
			following_state = Sync_state::TASK_STORE;
			logg("Done");
			break;
		case Sync_state::WAIT:
			if (request_is_done() )
			{
				auto response = outstanding_request->future_response.get();
				if (response.status_ok && response.http_code == HTTP_OK)
				{
					logg("Response arrived");
					state = following_state;
				}
				else
				{
					logg("Request failed");
					state = Sync_state::FAIL;
				}
			}
			break;
		case Sync_state::TASK_STORE:
			logg("Storing tasks");
			sync_tasks_to_database();
			state = Sync_state::TIME_REQUEST;
			logg("Done");
			break;
		case Sync_state::TIME_REQUEST:
			logg("Requesting times");
			outstanding_request = request_times(last_sync);
			state = Sync_state::WAIT;
			following_state = Sync_state::TIME_STORE;
			logg("Done");
			break;
		case Sync_state::TIME_STORE:
			logg("Storing times");
			sync_times_to_database();
			state = Sync_state::IDLE;
			next_sync = get_next_sync(current_sync);
			last_sync = current_sync;
			logg("Done");
			logg("Sync run done");
			break;
		case Sync_state::FAIL:
			logg("Failure");
			manage_network_problems();
			state = Sync_state::IDLE;
			next_sync = get_next_sync(current_sync);
			break;
	}

}

bool Sync_manager::request_is_done()
{
	auto future = outstanding_request->future_response;
	if(future.valid())
	{
		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	return false;
}


time_t Sync_manager::get_next_sync(time_t reference_point )
{

	auto sync_interval = settings_accessor.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	auto second_between_syncs = sync_interval * MINUTE;
	return reference_point + second_between_syncs;
}

bool Sync_manager::is_active()
{
	string base_url = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	return (base_url.length() > 0 && username.length() > 0);

}
void Sync_manager::sync_tasks_to_database()
{
	auto result = outstanding_request->future_response.get();
	vector<Task> tasks = to_tasks(result.response);

	vector<Task> tasks_to_update;
	for (Task task : tasks)
	{
		int64_t id = task_accessor.id(task.uuid);
		auto parent_uuid = task.parent_uuid;
		bool completed = task.completed;
		int64_t parent = 0;
		time_t last_changed = task.last_changed;
		string name  = task.name;
		auto uuid    = task.uuid;
		bool deleted = task.deleted;
		auto idle    = task.idle;
		bool quiet   = task.quiet;

		auto  original_task = task_accessor.by_id(id);
		if( original_task.has_value() )
		{
			//These are only to be stored locally
			idle  = original_task->idle;
			quiet = original_task->quiet;
		}
		if (parent_uuid)
		{
			parent = task_accessor.id(*parent_uuid);
			if (parent == 0)
			{
				tasks_to_update.push_back(task);
				continue;
			}
		}
		Task temp_task(name, parent, uuid, completed, id, last_changed, parent_uuid, deleted, idle, quiet);
		if (id > 0)
		{
			task_accessor.update(temp_task);
		}
		else
		{
			if( !temp_task.deleted )
			{
				task_accessor.create(temp_task);
			}
		}
	}
//Update tasks that had missing parent earlier
	for (Task task : tasks_to_update)
	{
		auto parent_uuid = task.parent_uuid;
		if (parent_uuid)
		{
			int64_t id         = task_accessor.id(task.uuid);
			int64_t parent     = task_accessor.id(*parent_uuid);
			bool    completed  = task.completed;
			time_t last_changed = task.last_changed;
			string name        = task.name;
			auto   uuid        = task.uuid;
			bool   deleted     = task.deleted;
			auto   idle        = task.idle;
			bool   quiet       = task.quiet;
			Task temp_task(name, parent, uuid, completed, id, last_changed, parent_uuid, deleted, idle, quiet );
			if (id > 0)
			{
				task_accessor.update(temp_task);
			}
			else
			{
				task_accessor.create(temp_task);
			}
		}
	}
	task_accessor.enable_notifications(true);
}

void Sync_manager::sync_times_to_database()
{
	auto result = outstanding_request->future_response.get();
	Time_list times = to_times(result.response);

	task_accessor.enable_notifications(false);

	for (auto item : times)
	{
		auto task_uuid = item.task_uuid;
		auto task_id = task_accessor.id(*task_uuid);
		if(task_id==0)
		{
			break;
		}
		auto uuid = item.uuid;
		auto id = time_accessor.uuid_to_id(uuid);
		time_t changed = item.changed;
		bool deleted = item.state == DELETED;
		time_t start = item.start;
		time_t stop = item.stop;
		bool running = false;
		if (id > 0)
		{
			auto original_item = time_accessor.by_id(id);
			if(original_item)
			{
				running = (original_item->state == RUNNING);
			}
		}

		Time_entry_state item_state = STOPPED;
		if ( running )
		{
			item_state = RUNNING;
		}
		if ( deleted )
		{
			item_state = DELETED;
		}

		Time_entry te(id, uuid, task_id, task_uuid, start, stop, item_state, changed);
		if (id > 0)
		{
			time_accessor.update(te);
		}
		else
		{
			if( te.state != DELETED)
			{
				time_accessor.create(te);
			}
		}
	}
	task_accessor.enable_notifications(true);
}

shared_ptr <asyncHTTPResponse> Sync_manager::request_tasks(time_t sincePointInTime)
{
	vector<Task> tasks = task_accessor.changed_since(sincePointInTime);
	string base_url = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	string password = settings_accessor.get_string("Password", DEFAULT_PASSWORD);
	string url = base_url + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignore_cert_error = settings_accessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	string json_string = to_json(tasks, username);
	return network.request(url, json_string, username, password, ignore_cert_error);
}

shared_ptr <asyncHTTPResponse> Sync_manager::request_times(time_t sincePointInTime)
{
	Time_list times = time_accessor.times_changed_since(sincePointInTime);
	std::string json_string = to_json(times);
	string base_url = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	string password = settings_accessor.get_string("Password", DEFAULT_PASSWORD);
	string url = base_url + "sync/times/" + username + "/" + std::to_string(sincePointInTime);
	bool ignore_cert_error = settings_accessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

	return network.request(url, json_string, username, password, ignore_cert_error);
}


void Sync_manager::manage_network_problems()
{
	auto result = outstanding_request->future_response.get();
	if (!result.status_ok || result.http_code != HTTP_OK)
	{
		std::stringstream text;
		// {} is replaced with the URI on which the connection failed
		text << fmt::format(_("Failed connection to {}:\n"), result.url.c_str());

		text << _("HTTP error ") << result.http_code << " ";
		if (result.http_code == HTTP_UNAUTHORIZED)
		{
			text << _("Username or password is wrong.");
		}
		else
		{
			text << result.error_message;
		}

		notifier.send(EventType::ERROR_MESSAGE, _("Network error"), text.str() );
	}
}

void Sync_manager::reset()
{
	next_full_sync = 0;
	next_sync = 0;
}


}
