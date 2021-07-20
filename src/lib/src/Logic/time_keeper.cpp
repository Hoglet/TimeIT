#include <libtimeit/timer.h>
#include <iostream>
#include <libtimeit/db/default_values.h>
#include "libtimeit/logic/time_keeper.h"
#include <libtimeit/utils.h>

using namespace std;
using namespace libtimeit;

time_keeper_observer::time_keeper_observer(Time_keeper& tk): time_keeper(tk)
{
	tk.attach(this);
}

time_keeper_observer::~time_keeper_observer()
{
	time_keeper.detach(this);
}

Time_keeper::Time_keeper(
		database& db,
		Timer& timer,
		notification_manager& notifier)
		:
		timer_observer(timer ),
		event_observer(notifier),
		times(db ),
		tasks(db ),
		settings(db ),
		idle_detector(timer)

{
	idle_gz = (int)settings.get_int("Gz", DEFAULT_GZ);
	default_idle_time = (unsigned)settings.get_int("Gt", DEFAULT_GT);
}

void Time_keeper::on_settings_changed(string name)
{
	if ( name == "Gt")
	{
		default_idle_time = (unsigned)settings.get_int("Gt", DEFAULT_GT);
	}
	if ( name == "Gz")
	{
		idle_gz = (int)settings.get_int("Gz", DEFAULT_GZ);
	}
}

void Time_keeper::on_signal_10_seconds()
{
	check_if_tasks_should_be_stopped();
	if ( user_is_active () )
	{
		update_running_entries();
	}
}


void Time_keeper::start(Task_id id)
{
	auto running_items = times.by_state(RUNNING);
	for (const auto& item: running_items)
	{
		if(item.owner == id)
		{
			return; //Already running
		}
	}

	auto now = libtimeit::now();
	times.create(Time_entry(id, now, now, RUNNING ) );
	notify_running_changed();
}

void Time_keeper::toggle(int64_t id)
{
	auto running_items = times.by_state(RUNNING);
	for (const auto& item: running_items)
	{
		if(item.owner == id)
		{
			stop_time(item);
			return;
		}
	}
	start(id);
}

void Time_keeper::stop_time(const Time_entry& time_entry)
{
	if( time_entry.stop -time_entry.start < idle_gz )
	{
		times.update(time_entry.with (DELETED));
	}
	else
	{
		times.update(time_entry.with(STOPPED));
	}
	notify_running_changed();
}

void Time_keeper::stop(Task_id id)
{
	auto running_items = times.by_state(RUNNING);
	for (const auto& item: running_items)
	{
		if(item.owner == id)
		{
			stop_time(item);
			return;
		}
	}
}

void Time_keeper::on_task_removed(Task_id id)
{
	auto running_items = times.by_state(RUNNING);
	for (const auto& item: running_items)
	{
		if(item.owner == id)
		{
			stop_time(item);
		}
	}
}
void Time_keeper::on_complete_update()
{
	//TODO Detect task removal during syncing
}



bool Time_keeper::has_running_tasks()
{
	auto running_items = times.by_state(RUNNING);
	return ( !running_items.empty() );
}

void Time_keeper::stop_all()
{
	auto running_items = times.by_state(RUNNING);
	for (const auto& item: running_items)
	{
		stop_time(item);
	}
}


[[nodiscard]] bool  Time_keeper::tasks_are_running() const
{
	auto running_items = times.by_state(RUNNING);
	return running_items.empty();
}

[[maybe_unused]] [[nodiscard]] bool   Time_keeper::is_idle()
{
	return idle_detector.time_idle() > 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void Time_keeper::notify_running_changed()
{
	std::list<time_keeper_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		time_keeper_observer* observer = *iter;
		observer->on_running_changed();
	}
}

void Time_keeper::notify_idle_detected(const Time_entry& te)
{
	for (auto* observer: observers)
	{
		observer->on_idle_detected(te);
	}
}

void Time_keeper::attach(time_keeper_observer* observer)
{
	observers.push_back(observer);
}
void Time_keeper::detach(time_keeper_observer* observer)
{
	observers.remove(observer);
}

void Time_keeper::check_for_status_change()
{
	auto currently_running = times.currently_running();
	if(currently_running != old_running)
	{
		notify_running_changed();
	}
	old_running = currently_running;
}
void Time_keeper::on_time_entry_changed(const Time_entry& /*id*/)
{
	check_for_status_change();
}

bool Time_keeper::user_is_active()
{
	return 	(idle_detector.time_idle() < 10);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void Time_keeper::update_running_entries()
{
	auto running = times.by_state(RUNNING);
	for (const auto& time_entry: running)
	{
		auto updated_time_entry = time_entry.with_stop(libtimeit::now());
		times.update(updated_time_entry );
	}

}

void Time_keeper::check_if_tasks_should_be_stopped()
{
	auto now = libtimeit::now();
	list<Time_entry> times_to_stop {};
	auto running_time_items = times.by_state(RUNNING);
	for (const auto& time_item: running_time_items)
	{
		auto owner = tasks.by_id(time_item.owner);
		auto idle_time = owner->idle;
		if(idle_time == 0)
		{
			idle_time = default_idle_time;
		}
		auto time_inactive = now - time_item.stop;
		if( time_inactive > idle_time * MINUTE )
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

