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



sync_manager::sync_manager(
		database &db,
		abstract_network  &op_network,
		notification_manager& notifier_,
		Timer&    timer
		):
		timer_observer(timer),
		tasks(db),
		times(db),
		settings(db),
		network(op_network),
		notifier(notifier_)
{
}


Sync_state sync_manager::status()
{
	return state;
}

void sync_manager::on_signal_1_second()
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

bool sync_manager::request_is_done()
{
	auto future = outstanding_request->future_response;
	if(future.valid())
	{
		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	return false;
}


time_t sync_manager::get_next_sync(time_t reference_point )
{

	auto sync_interval = settings.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	auto second_between_syncs = sync_interval * MINUTE;
	return reference_point + second_between_syncs;
}

bool sync_manager::is_active()
{
	string base_url = settings.get_string("URL", DEFAULT_URL);
	string username = settings.get_string("Username", DEFAULT_USER);
	return (base_url.length() > 0 && username.length() > 0);

}
void sync_manager::sync_tasks_to_database()
{
	auto result = outstanding_request->future_response.get();
	vector<task> tasks_to_sync = to_tasks(result.response);

	vector<task> tasks_to_update;
	for (task item : tasks_to_sync)
	{
		int64_t old_id           = tasks.to_id( item.id);
		auto    parent_uuid  = item.parent_id;
		int64_t parent       = 0;
		auto    last_changed = item.last_changed;
		string  name         = item.name;
		auto    id           = item.id;
		bool    deleted      = item.deleted;
		auto    idle         = item.idle;
		bool    quiet        = item.quiet;

		auto  original_task = tasks.by_id(item.id);
		if( original_task.has_value() )
		{
			//These are only to be stored locally
			idle  = original_task->idle;
			quiet = original_task->quiet;
		}
		if (parent_uuid)
		{
			parent = tasks.to_id(*parent_uuid);
			if (parent == 0)
			{
				tasks_to_update.push_back(item);
				continue;
			}
		}
		task temp_task( name, id, last_changed, parent_uuid, deleted, idle, quiet);
		if ( old_id > 0)
		{
			tasks.update(temp_task);
		}
		else
		{
			if( !temp_task.deleted )
			{
				tasks.create(temp_task);
			}
		}
	}
//Update tasks that had missing parent earlier
	for (task item : tasks_to_update)
	{
		auto parent_uuid = item.parent_id;
		if (parent_uuid)
		{
			int64_t old_id       = tasks.to_id(item.id);
			auto    last_changed = item.last_changed;
			string  name         = item.name;
			auto    id           = item.id;
			bool    deleted      = item.deleted;
			auto    idle         = item.idle;
			bool    quiet        = item.quiet;

			task temp_task( name, id, last_changed, parent_uuid, deleted, idle, quiet );
			if (old_id > 0)
			{
				tasks.update(temp_task);
			}
			else
			{
				tasks.create(temp_task);
			}
		}
	}
	tasks.enable_notifications(true);
}

void sync_manager::sync_times_to_database()
{
	auto result = outstanding_request->future_response.get();
	Time_list times_to_sync = to_times(result.response);

	tasks.enable_notifications(false);

	for (auto item : times_to_sync)
	{
		auto task_uuid = item.owner_id;
		auto owner = tasks.to_id(task_uuid);
		if(owner == 0)
		{
			continue;
		}
		time_t changed = item.changed;
		bool deleted = item.state == DELETED;
		time_t start = item.start;
		time_t stop = item.stop;
		bool running = false;
		auto comment = item.comment;

		auto original_item = times.by_id(item.id);
		if(original_item)
		{
			running = (original_item->state == RUNNING);
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

		Time_entry te( item.id, task_uuid, start, stop, item_state, changed, comment);
		if (original_item.has_value())
		{
			times.update(te);
		}
		else
		{
			if( te.state != DELETED && (te.stop - te.start)>60 )
			{
				times.create(te);
			}
		}
	}
	tasks.enable_notifications(true);
}

shared_ptr <async_http_response> sync_manager::request_tasks(time_t sincePointInTime)
{
	vector<task> changed_tasks = tasks.changed_since(sincePointInTime);
	string base_url = settings.get_string("URL", DEFAULT_URL);
	string username = settings.get_string("Username", DEFAULT_USER);
	string password = settings.get_string("Password", DEFAULT_PASSWORD);
	string url = base_url + "sync/tasks/" + username + "/" + std::to_string(sincePointInTime);
	bool ignore_cert_error = settings.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	string json_string = to_json(changed_tasks, username);
	return network.request(url, json_string, username, password, ignore_cert_error);
}

shared_ptr <async_http_response> sync_manager::request_times(time_t sincePointInTime)
{
	Time_list changed_times = times.times_changed_since(sincePointInTime);
	std::string json_string = to_json(changed_times);
	string base_url = settings.get_string("URL", DEFAULT_URL);
	string username = settings.get_string("Username", DEFAULT_USER);
	string password = settings.get_string("Password", DEFAULT_PASSWORD);
	string url = base_url + "sync/times/" + username + "/" + std::to_string(sincePointInTime);
	bool ignore_cert_error = settings.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);

	return network.request(url, json_string, username, password, ignore_cert_error);
}


void sync_manager::manage_network_problems()
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
			auto logg_text = fmt::format("Error {} requesting {} : {}",
										 result.http_code,
										 result.url.c_str(),
										 result.response);
			logg(logg_text);
		}

		notifier.send(EventType::ERROR_MESSAGE, _("Network error"), text.str() );
	}
}

void sync_manager::reset()
{
	next_full_sync = 0;
	next_sync = 0;
}


}
