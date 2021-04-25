#include <libtimeit/sync/sync_manager.h>

#include <libtimeit/utils.h>
#include <libtimeit/sync/json.h>
#include <libtimeit/db/default_values.h>
#include <sstream>
#include <libintl.h>

#include <libtimeit/internal/gettext.h>

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
		Notifier& notifier,
		Timer&    timer
		):
		notifier_(notifier),
		network(op_network),
		task_accessor(database),
		time_accessor(database),
		settings_accessor(database),
		Timer_observer(timer)

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
				state = Sync_state::TASK_REQUEST;
				current_sync = now;
				next_sync = get_next_sync(current_sync);
			}
		}
			break;
		case Sync_state::TASK_REQUEST:
			outstandingRequest = request_tasks(last_sync);
			state = Sync_state::WAIT;
			following_state = Sync_state::TASK_STORE;
			break;
		case Sync_state::WAIT:
			if (request_is_done() )
			{
				auto response = outstandingRequest->futureResponse.get();
				if (response.status_OK && response.http_code == HTTP_OK)
				{
					state = following_state;
				}
				else
				{
					state = Sync_state::FAIL;
				}
			}
			break;
		case Sync_state::TASK_STORE:
			sync_tasks_to_database();
			state = Sync_state::TIME_REQUEST;
		case Sync_state::TIME_REQUEST:
			outstandingRequest = request_times(last_sync);
			state = Sync_state::WAIT;
			following_state = Sync_state::TIME_STORE;
			break;
		case Sync_state::TIME_STORE:
			sync_times_to_database();
			state = Sync_state::IDLE;
			next_sync = get_next_sync(current_sync);
			last_sync = current_sync;
			break;
		case Sync_state::FAIL:
			manage_network_problems();
			state = Sync_state::IDLE;
			next_sync = get_next_sync(current_sync);
			break;
	}

}

bool Sync_manager::request_is_done()
{
	auto future = outstandingRequest->futureResponse;
	if(future.valid())
	{
		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	return false;
}


time_t Sync_manager::get_next_sync(time_t referencePoint )
{

	int syncInterval = settings_accessor.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	int secondBetweenSyncs = syncInterval * MINUTE;
	return referencePoint + secondBetweenSyncs;
}

bool Sync_manager::is_active()
{
	string baseUrl = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	return (baseUrl.length() > 0 && username.length() > 0);
}
void Sync_manager::sync_tasks_to_database()
{
	auto result = outstandingRequest->futureResponse.get();
	vector<Task> tasks = to_tasks(result.response);

	vector<Task> tasksToUpdate;
	for (Task task : tasks)
	{
		int64_t id = task_accessor.ID(task.uuid);
		auto parentUUID = task.parent_uuid;
		bool completed = task.completed;
		int64_t parent = 0;
		time_t lastChanged = task.last_changed;
		string name  = task.name;
		auto uuid    = task.uuid;
		bool deleted = task.deleted;
		int  idle    = task.idle;
		bool quiet   = task.quiet;

		auto  original_task = task_accessor.by_ID(id);
		if( original_task.has_value() )
		{
			//These are only to be stored locally
			idle  = original_task->idle;
			quiet = original_task->quiet;
		}
		if (parentUUID)
		{
			parent = task_accessor.ID(*parentUUID);
			if (parent == 0)
			{
				tasksToUpdate.push_back(task);
				continue;
			}
		}
		Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted, idle, quiet);
		if (id > 0)
		{
			task_accessor.update(tempTask);
		}
		else
		{
			task_accessor.create(tempTask);
		}
	}
//Update tasks that had missing parent earlier
	for (Task task : tasksToUpdate)
	{
		auto parentUUID = task.parent_uuid;
		if (parentUUID)
		{
			int64_t id         = task_accessor.ID(task.uuid);
			int64_t parent     = task_accessor.ID(*parentUUID);
			bool    completed  = task.completed;
			time_t lastChanged = task.last_changed;
			string name        = task.name;
			auto   uuid        = task.uuid;
			bool   deleted     = task.deleted;
			int    idle        = task.idle;
			bool   quiet       = task.quiet;
			Task tempTask(name, parent, uuid, completed, id, lastChanged, parentUUID, deleted, idle, quiet );
			if (id > 0)
			{
				task_accessor.update(tempTask);
			}
			else
			{
				task_accessor.create(tempTask);
			}
		}
	}
	task_accessor.enable_notifications(true);
}

void Sync_manager::sync_times_to_database()
{
	auto resultA = outstandingRequest->futureResponse.get();
	Time_list times = to_times(resultA.response);

	task_accessor.enable_notifications(false);

	for (auto item : times)
	{
		auto taskUUID = item.task_uuid;
		int64_t taskID = task_accessor.ID(*taskUUID);
		auto uuid = item.uuid;
		auto id = time_accessor.uuid_to_id(uuid);
		time_t changed = item.changed;
		bool deleted = item.state == DELETED;
		time_t start = item.start;
		time_t stop = item.stop;
		bool running = false;
		if (id > 0)
		{
			auto originalItem = time_accessor.by_ID(id);
			if(originalItem)
			{
				running = originalItem->running();
			}
		}

		Time_entry_state state_ = STOPPED;
		if ( running )
		{
			state_ = RUNNING;
		}
		if ( deleted )
		{
			state_ = DELETED;
		}

		Time_entry te(id, uuid, taskID, taskUUID, start, stop, state_, changed);
		if (id > 0)
		{
			time_accessor.update(te);
		}
		else
		{
			time_accessor.create(te);
		}
	}
	task_accessor.enable_notifications(true);
}

shared_ptr <asyncHTTPResponse> Sync_manager::request_tasks(time_t sincePointInTime)
{
	vector<Task> tasks = task_accessor.changed_since(sincePointInTime);
	string baseUrl = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	string password = settings_accessor.get_string("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settings_accessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	string jsonString = to_json(tasks, username);
	return network.request(url, jsonString, username, password, ignoreCertError);
}

shared_ptr <asyncHTTPResponse> Sync_manager::request_times(time_t sincePointInTime)
{
	Time_list times = time_accessor.times_changed_since(sincePointInTime);
	std::string jsonString = to_json(times);
	string baseUrl = settings_accessor.get_string("URL", DEFAULT_URL);
	string username = settings_accessor.get_string("Username", DEFAULT_USER);
	string password = settings_accessor.get_string("Password", DEFAULT_PASSWORD);
	string url = baseUrl + "sync/times/" + username + "/" + std::to_string(sincePointInTime);
	bool ignoreCertError = settings_accessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

	return network.request(url, jsonString, username, password, ignoreCertError);
}


void Sync_manager::manage_network_problems()
{
	auto result = outstandingRequest->futureResponse.get();
	if ( !result.status_OK || result.http_code != HTTP_OK)
	{
		std::stringstream text;
		// %s is replaced with the URI on which the connection failed
		text << string_printf(_("Failed connection to %s:\n"), result.url.c_str());

		text << _("HTTP error ") << result.http_code << " ";
		if (result.http_code == HTTP_UNAUTHORIZED)
		{
			text << _("Username or password is wrong.");
		}
		else
		{
			text << result.error_message;
		}

		notifier_.send(EventType::ERROR_MESSAGE, _("Network error"), text.str() );
	}
}

void Sync_manager::reset()
{
	next_full_sync = 0;
	next_sync = 0;
}


}
