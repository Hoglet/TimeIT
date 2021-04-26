#include <libtimeit/timer.h>
#include <iostream>
#include <libtimeit/db/default_values.h>
#include "libtimeit/logic/time_keeper.h"
#include <libtimeit/utils.h>

using namespace std;
using namespace libtimeit;


Time_keeper::Time_keeper(
		Database& database,
		Timer& timer,
		Notifier& notifier)
		:
		time_accessor( database ),
		task_accessor( database ),
		settings_accessor( database ),
		Event_observer(notifier),
		Timer_observer(timer ),
		idle_detector(timer)

{
	int i = 4;
	auto running = time_accessor.by_state( RUNNING );
	for ( auto item: running )
	{
		time_accessor.update( item.with( STOPPED ));
	}
	idle_Gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
	default_idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
	idle_detector.idle_timeout(default_idle_time);
}

void Time_keeper::on_settings_changed(string name)
{
	if ( name == "Gt")
	{
		default_idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
		idle_detector.idle_timeout(default_idle_time);
	}
	if ( name == "Gz")
	{
		idle_Gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
	}
}

void Time_keeper::on_signal_10_seconds()
{
	the_method_that_do_stuff();
}


void Time_keeper::start(Task_ID id)
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		if( item.task_ID == id)
		{
			return; //Already running
		}
	}
	auto now = libtimeit::now();
	time_accessor.create(Time_entry( id, now, now, RUNNING ) );
	notify_running_changed();
}

void Time_keeper::toggle(int64_t id)
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		if( item.task_ID == id)
		{
			stop_time(item.ID);
			return;
		}
	}
	start(id);
}

void Time_keeper::stop_time(const Time_ID id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		if( time_entry->stop -time_entry->start < MINUTE )
		{
			time_accessor.update( time_entry->with (DELETED));
		}
		else
		{
			time_accessor.update( time_entry->with(STOPPED));
		}
	}
	notify_running_changed();
}

void Time_keeper::stop(Task_ID id)
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		if( item.task_ID == id)
		{
			stop_time(item.ID);
			return;
		}
	}
}

void Time_keeper::on_task_removed(Task_ID id)
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		if( item.task_ID == id)
		{
			stop_time(item.ID);
		}
	}
}
void Time_keeper::on_complete_update()
{
	//TODO Detect task removal during syncing
}



bool Time_keeper::hasRunningTasks()
{
	auto running_items = time_accessor.by_state(RUNNING);
	return ( !running_items.empty() );
}

void Time_keeper::stop_all()
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		stop_time(item.ID);
	}
}

void Time_keeper::stop_all_and_continue()
{
	auto running_items = time_accessor.by_state(RUNNING);
	for (auto item: running_items)
	{
		stop_time(item.ID);
		start(item.task_ID);
	}
}

[[nodiscard]] bool  Time_keeper::is_idle() const
{
	auto running_items = time_accessor.by_state(RUNNING);
	return running_items.empty();
}

time_t Time_keeper::time_idle()
{
	return idle_detector.time_idle();
}

void Time_keeper::notify_running_changed()
{
	std::list<Time_keeper_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Time_keeper_observer* observer = *iter;
		observer->on_running_changed();
	}
}

void Time_keeper::notify_idle_detected(Time_ID id)
{
	for (auto observer: observers)
	{
		observer->on_idle_detected(id);
	}
}

void Time_keeper::notify_activity_resumed()
{
	for (auto observer : observers)
	{
		observer->on_activity_resumed();
	}

}



void Time_keeper::attach(Time_keeper_observer* observer)
{
	observers.push_back(observer);
}
void Time_keeper::detach(Time_keeper_observer* observer)
{
	observers.remove(observer);
}

void Time_keeper::the_method_that_do_stuff()
{
	if ( user_is_active () )
	{
		update_running_entries();
	}
	else
	{
		check_if_tasks_should_be_stopped();
	}
}

bool Time_keeper::user_is_active()
{
	return 	(idle_detector.minutes_idle() < idle_Gz);
}

void Time_keeper::update_running_entries()
{
	auto running = time_accessor.by_state(RUNNING);
	for (auto time_entry: running)
	{
		auto updated_time_entry = time_entry.with_stop(libtimeit::now());
		time_accessor.update( updated_time_entry );
	}

}

void Time_keeper::check_if_tasks_should_be_stopped()
{
	auto time_inactive = idle_detector.time_idle();
	list<Time_ID> times_to_stop {};
	auto running_time_items = time_accessor.by_state(RUNNING);
	for (auto time_item: running_time_items)
	{
		auto task = task_accessor.by_ID(time_item.task_ID);
		auto idle_time = task->idle;
		if(idle_time == 0)
		{
			idle_time = default_idle_time;
		}
		if( time_inactive > idle_time )
		{
			times_to_stop.emplace_back(time_item.ID );
			if( ! task->quiet )
			{
				notify_idle_detected(time_item.ID );
			}
		}
	}
	for ( auto time_id: times_to_stop )
	{
		auto time_entry = time_accessor.by_ID(time_id);
		if ( time_entry.has_value() && time_entry->state == RUNNING )
		{
			stop_time(time_id);
		}
	}
}

