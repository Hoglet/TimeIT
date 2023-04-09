#include <libtimeit/timer.h>
#include <iostream>
#include <libtimeit/db/defaults.h>
#include "libtimeit/logic/time_keeper.h"
#include <libtimeit/utils.h>

using namespace std;
using namespace libtimeit;

time_manager_observer::time_manager_observer( time_manager& tk): time_keeper( tk)
{
	tk.attach(this);
}

time_manager_observer::~time_manager_observer()
{
	time_keeper.detach(this);
}

time_manager::time_manager(
		database& db,
		timer_base& timer,
		notification_manager& notifier)
		:
		timer_observer(timer ),
		event_observer(notifier),
		times(db ),
		tasks(db ),
		settings(db )
{
	idle_gz = minutes( settings.get_int( "Gz", defaults::g_zero) );
	default_idle_time = minutes(settings.get_int( "Gt", defaults::g_time));
}

void time_manager::on_settings_changed( string name)
{
	if ( name == "Gt")
	{
		default_idle_time = minutes(settings.get_int( "Gt", defaults::g_time));
	}
	if ( name == "Gz")
	{
		idle_gz = minutes( settings.get_int( "Gz", defaults::g_zero) );
	}
}

void time_manager::on_signal_10_seconds()
{
	check_if_tasks_should_be_stopped();
	if ( user_is_active () )
	{
		update_running_entries();
	}
}


void time_manager::start( task_id id)
{
	auto running_items = times.by_state(running);
	for (const auto& item: running_items)
	{
		if( item.owner_id == id)
		{
			return; //Already running
		}
	}

	auto now = system_clock::now();
	times.create( time_entry( id, now, now, running ) );
	notify_running_changed();
}

void time_manager::toggle( task_id id)
{
	auto running_items = times.by_state(running);
	for (const auto& item: running_items)
	{
		if( item.owner_id == id)
		{
			stop_time(item);
			return;
		}
	}
	start(id);
}

void time_manager::stop_time( const time_entry& item)
{
	if( item.stop - item.start < idle_gz )
	{
		times.update( item.with ( deleted));
	}
	else
	{
		times.update( item.with( stopped));
	}
	notify_running_changed();
}

void time_manager::stop( task_id id)
{
	auto running_items = times.by_state(running);
	for (const auto& item: running_items)
	{
		if( item.owner_id == id)
		{
			stop_time(item);
			return;
		}
	}
}

void time_manager::on_task_removed( const task& removed_task)
{
	auto running_items = times.by_state(running);
	for (const auto& item: running_items)
	{
		if( item.owner_id == removed_task.id)
		{
			stop_time(item);
		}
	}
}
void time_manager::on_complete_update()
{
	//TODO Detect task removal during syncing
}



bool time_manager::has_running_tasks()
{
	auto running_items = times.by_state(running);
	return ( !running_items.empty() );
}

void time_manager::stop_all()
{
	auto running_items = times.by_state(running);
	for (const auto& item: running_items)
	{
		stop_time(item);
	}
}


[[nodiscard]] bool  time_manager::tasks_are_running() const
{
	auto running_items = times.by_state(running);
	return running_items.empty();
}

[[maybe_unused]] [[nodiscard]] bool   time_manager::is_idle()
{
	return system.time_idle() > 10s;
}

void time_manager::notify_running_changed()
{
	std::list<time_manager_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		time_manager_observer* observer = *iter;
		observer->on_running_changed();
	}
}

void time_manager::notify_idle_detected( const time_entry& te)
{
	for (auto* observer: observers)
	{
		observer->on_idle_detected(te);
	}
}

void time_manager::attach( time_manager_observer* observer)
{
	observers.push_back(observer);
}
void time_manager::detach( time_manager_observer* observer)
{
	observers.remove(observer);
}

void time_manager::check_for_status_change()
{
	auto currently_running = times.currently_running();
	if(currently_running != old_running)
	{
		notify_running_changed();
	}
	old_running = currently_running;
}
void time_manager::on_time_entry_changed( const time_entry& /*id*/)
{
	check_for_status_change();
}

bool time_manager::user_is_active()
{
	return 	(system.time_idle() < 10s);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void time_manager::update_running_entries()
{
	auto running = times.by_state( time_entry_state::running);
	for (const auto& item: running)
	{
		auto updated_time_entry = item.with_stop( system_clock::now());
		times.update(updated_time_entry );
	}

}

void time_manager::check_if_tasks_should_be_stopped()
{
	auto now = system_clock::now();
	list<time_entry> times_to_stop {};
	auto running_time_items = times.by_state(running);
	for (const auto& time_item: running_time_items)
	{
		auto owner = tasks.by_id(time_item.owner_id);
		auto idle_time = owner->idle;
		if(idle_time == 0min)
		{
			idle_time = default_idle_time;
		}
		auto time_inactive = now - time_item.stop;
		if( time_inactive > duration_cast<seconds>(idle_time) )
		{
			times_to_stop.push_back( time_item );
			if( ! owner->quiet )
			{
				notify_idle_detected( time_item );
			}
		}
	}
	for ( const auto& time_to_stop: times_to_stop )
	{
		stop_time(time_to_stop);
	}
}
