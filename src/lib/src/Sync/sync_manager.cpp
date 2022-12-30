#include <libtimeit/sync/sync_manager.h>

#include <libtimeit/utils.h>
#include <libtimeit/db/defaults.h>
#include <sstream>
#include <libintl.h>
#include <fmt/core.h>
#include <libtimeit/internal/gettext.h>
#include <libtimeit/logging.h>

namespace libtimeit
{

static const int http_ok = 200;
static const int http_unauthorized = 401;
static const int minute = 60;

using namespace std;

const int one_day = 60 * 60 * 24;



sync_manager::sync_manager(
		database &db,
		abstract_network  &op_network,
		notification_manager& op_notifier,
		timer_base&    timer
		):
		timer_observer(timer),
		tasks(db),
		times(db),
		settings(db),
		network(op_network),
		notifier( op_notifier)
{
}


sync_state sync_manager::status()
{
	return state;
}

void sync_manager::on_signal_1_second()
{
	switch (state)
	{
		case sync_state::idle:
		{
			auto now = system_clock::now();
			if (is_active() && now > next_full_sync)
			{
				next_full_sync = now + 24h;
				last_sync = system_clock::from_time_t(0);
			}
			if (is_active() && now > next_sync)
			{
				logg("Starting sync run");
				state = sync_state::task_request;
				current_sync = now;
				next_sync = get_next_sync(current_sync);
			}
		}
			break;
		case sync_state::task_request:
			logg("Requesting tasks");
			outstanding_request = request_tasks( last_sync );
			state = sync_state::wait;
			following_state = sync_state::task_store;
			logg("Done");
			break;
		case sync_state::wait:
			if (request_is_done() )
			{
				auto response = outstanding_request->future_response.get();
				if (response.status_ok && response.http_code == http_ok)
				{
					logg("Response arrived");
					state = following_state;
				}
				else
				{
					logg("Request failed");
					state = sync_state::fail;
				}
			}
			break;
		case sync_state::task_store:
			logg("Storing tasks");
			sync_tasks_to_database();
			state = sync_state::time_request;
			logg("Done");
			break;
		case sync_state::time_request:
			logg("Requesting times");
			outstanding_request = request_times(last_sync);
			state = sync_state::wait;
			following_state = sync_state::time_store;
			logg("Done");
			break;
		case sync_state::time_store:
			logg("Storing times");
			sync_times_to_database();
			state = sync_state::idle;
			next_sync = get_next_sync(current_sync);
			last_sync = current_sync;
			logg("Done");
			logg("Sync run done");
			break;
		case sync_state::fail:
			logg("Failure");
			manage_network_problems();
			state = sync_state::idle;
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


time_point<system_clock> sync_manager::get_next_sync(time_point<system_clock> reference_point )
{

	auto sync_interval = minutes(settings.get_int( "SyncInterval", defaults::sync_interval));
	return reference_point + sync_interval;
}

bool sync_manager::is_active()
{
	string base_url = settings.get_string("URL", defaults::url);
	string username = settings.get_string( "Username", defaults::user);
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
	time_list times_to_sync = to_times( result.response);

	tasks.enable_notifications(false);

	for (auto item : times_to_sync)
	{
		auto task_uuid = item.owner_id;
		auto owner = tasks.to_id(task_uuid);
		if(owner == 0)
		{
			continue;
		}
		auto changed = item.changed;
		bool deleted = item.state == time_entry_state::deleted;
		auto   start = item.start;
		auto    stop = item.stop;
		bool running = false;
		auto comment = item.comment;

		auto original_item = times.by_id(item.id);
		if(original_item)
		{
			running = (original_item->state == time_entry_state::running);
		}

		time_entry_state item_state = stopped;
		if ( running )
		{
			item_state = time_entry_state::running;
		}
		if ( deleted )
		{
			item_state = time_entry_state::deleted;
		}

		time_entry te( item.id, task_uuid, start, stop, item_state, changed, comment);
		if (original_item.has_value())
		{
			times.update(te);
		}
		else
		{
			if( te.state != time_entry_state::deleted && (te.stop - te.start) > 60s )
			{
				times.create(te);
			}
		}
	}
	tasks.enable_notifications(true);
}

shared_ptr <async_http_response> sync_manager::request_tasks(time_point<system_clock> since_point_in_time )
{
	vector<task> changed_tasks = tasks.changed_since( since_point_in_time );
	string base_url = settings.get_string("URL", defaults::url);
	string username = settings.get_string( "Username", defaults::user);
	string password = settings.get_string( "Password", defaults::password);
	string url = base_url + "sync/tasks/" + username + "/" + std::to_string( system_clock::to_time_t(since_point_in_time) );
	bool ignore_cert_error = settings.get_bool( "IgnoreCertErr", defaults::ignore_cert_err);
	string json_string = to_json(changed_tasks, username);
	return network.request(url, json_string, username, password, ignore_cert_error);
}

shared_ptr <async_http_response> sync_manager::request_times( time_point<system_clock> since_point_in_time )
{
	time_list changed_times = times.times_changed_since( since_point_in_time );
	std::string json_string = to_json(changed_times);
	string base_url = settings.get_string("URL", defaults::url);
	string username = settings.get_string( "Username", defaults::user);
	string password = settings.get_string( "Password", defaults::password);
	string url = base_url + "sync/times/" + username + "/" + std::to_string( system_clock::to_time_t( since_point_in_time ));
	bool ignore_cert_error = settings.get_bool( "IgnoreCertErr", defaults::ignore_cert_err);

	return network.request(url, json_string, username, password, ignore_cert_error);
}


void sync_manager::manage_network_problems()
{
	auto result = outstanding_request->future_response.get();
	if (!result.status_ok || result.http_code != http_ok)
	{
		std::stringstream text;
		// {} is replaced with the URI on which the connection failed
		text << fmt::format(_("Failed connection to {}:\n"), result.url.c_str());

		text << _("HTTP error ") << result.http_code << " ";
		if (result.http_code == http_unauthorized)
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

		notifier.send( event_type::error_message, _( "Network error"), text.str() );
	}
}

void sync_manager::reset()
{
	next_full_sync = system_clock::from_time_t(0);
	next_sync      = system_clock::from_time_t(0);
}


}
