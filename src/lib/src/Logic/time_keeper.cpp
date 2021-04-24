#include <libtimeit/timer.h>
#include <iostream>
#include <libtimeit/db/default_values.h>
#include "libtimeit/logic/time_keeper.h"
#include <libtimeit/utils.h>

using namespace std;
using namespace libtimeit;


//LCOV_EXCL_START
void Time_keeper::on_signal_1_second()
{
	auto idle=idle_detector.idle();

	if(idle != is_idle_)
	{
		is_idle_ = idle;
		if(idle)
		{
			notify_idle_detected();
		}
		else
		{
			notify_activity_resumed();
		}
	}
}
//LCOV_EXCL_STOP

Time_keeper::Time_keeper(
		Database& database,
		Timer& timer,
		Notifier& notifier)
		:
		time_accessor( database ),
		settings_accessor( database ),
		Event_observer(notifier),
		Timer_observer(timer ),
		idle_detector(timer)

{
	time_accessor.stop_all();
	idle_Gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
	auto idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
	idle_detector.idle_timeout(idle_time);
}

Time_keeper::~Time_keeper()
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.begin();
	while (it != active_tasks.end())
	{
		stop(it->second.task_ID);
		it = active_tasks.begin();
	}
	//settings_accessor.detach(this);
}

void Time_keeper::on_settings_changed(string name)
{
	if ( name == "Gt")
	{
		auto idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
		idle_detector.idle_timeout(idle_time);
	}
	if ( name == "Gz")
	{
		idle_Gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
	}
}

void Time_keeper::on_signal_10_seconds()
{
	if (enabled)
	{
		map<int64_t, Task_time>::iterator it;
		if (idle_detector.minutes_idle() < idle_Gz)
		{
			//Only saving time when being certain that somebody is working
			for (it = active_tasks.begin(); it != active_tasks.end(); ++it)
			{
				Task_time tt = it->second;
				update_task(tt.task_ID);
			}
		}
	}
}

void Time_keeper::start(int64_t id)
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.find(id);
	if (it == active_tasks.end())
	{

		Task_time task{};
		task.start = libtimeit::now();
		task.stop = libtimeit::now();
		task.time_ID = time_accessor.create(Time_entry(id, task.start, task.stop ) );
		task.task_ID = id;
		active_tasks[id] = task;
		time_accessor.setRunning(task.time_ID, true);
		notify_running_changed();
	}
}

void Time_keeper::toggle(int64_t id)
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.find(id);
	if (it == active_tasks.end())
	{
		start(id);
	}
	else
	{
		stop(id);
	}
}

void Time_keeper::stop(int64_t id)
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		Task_time task = it->second;
		active_tasks.erase(it);
		auto result = time_accessor.by_ID(task.time_ID);
		if(result.has_value())
		{
			auto original_entry = *result;
			time_accessor.update( original_entry.with( Time_entry_state::STOPPED ));
			time_accessor.setRunning(task.time_ID, false);
			if (task.stop - task.start < MINUTE)
			{
				time_accessor.remove(task.time_ID);
			}
		}
		notify_running_changed();
	}
}

void Time_keeper::on_task_removed(int64_t id)
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		active_tasks.erase(it);
		notify_running_changed();
	}
}
void Time_keeper::on_complete_update()
{
	//TODO Detect task removal during syncing
}

void Time_keeper::update_task(int64_t id)
{
	map<int64_t, Task_time>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		it->second.stop = libtimeit::now();
		Task_time task = it->second;

		auto te = time_accessor.by_ID(task.time_ID);
		if(te)
		{
			time_accessor.update(te->with_stop(task.stop));
		}
	}
}

bool Time_keeper::hasRunningTasks()
{
	return ( !active_tasks.empty() );
}

void Time_keeper::stop_all()
{
	map<int64_t, Task_time>::iterator it;
	auto copy_of_active_tasks = active_tasks;
	for (it = copy_of_active_tasks.begin(); it != copy_of_active_tasks.end(); ++it)
	{
		Task_time tt = it->second;
		stop(tt.task_ID);
	}
}

void Time_keeper::stop_all_and_continue()
{
	map<int64_t, Task_time>::iterator it;
	map<int64_t, Task_time> copy_of_active_tasks = active_tasks;
	for (it = copy_of_active_tasks.begin(); it != copy_of_active_tasks.end(); ++it)
	{
		Task_time tt = it->second;
		stop(tt.task_ID);
		start(tt.task_ID);
	}
}

bool Time_keeper::is_idle() const
{
	return is_idle_;
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

void Time_keeper::notify_idle_detected()
{
	std::list<Time_keeper_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Time_keeper_observer* observer = *iter;
		observer->on_idle_detected();
	}
}

void Time_keeper::notify_activity_resumed()
{
	std::list<Time_keeper_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Time_keeper_observer* observer = *iter;
		observer->on_activity_resumed();
	}

}

void Time_keeper::enable(bool enable)
{
	enabled = enable;
	if (enabled)
	{
		idle_detector.reset();
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
