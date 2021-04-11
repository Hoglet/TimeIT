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
	// std::cout << "Timekeeper::on_signal_1_second() was m_isIdle " << m_isIdle << " will be idle " << idle << std::endl;
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
		Timer& op_timer,
		Notifier& notifier)
		:
		time_accessor( database ),
		settings_accessor( database ),
		timer( op_timer ),
		Event_observer(notifier),
		Timer_observer(op_timer ),
		idle_detector(op_timer)

{
	time_accessor.stop_all();

	on_settings_changed("");
	enabled = true;
	is_idle_ = false;
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
	if (name.length() < 1 || name == "Gt")
	{
		long idleGt = settings_accessor.get_int("Gt", DEFAULT_GT);
		idle_detector.idle_timeout(idleGt);
	}
	if (name.length() < 1 || name == "Gz")
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
		Task_time task;
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
			if (task.stop - task.start < 60)
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

void Time_keeper::update_task(int64_t id, time_t now)
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

void Time_keeper::update_task(int64_t id)
{
	time_t now = libtimeit::now();
	update_task(id, now);
}

bool Time_keeper::is_active_task(int64_t taskID)
{
	return active_tasks.find(taskID) != active_tasks.end();
}
bool Time_keeper::hasRunningTasks()
{
	return (active_tasks.empty() == false);
}

void Time_keeper::stop_all()
{
	map<int64_t, Task_time>::iterator it;
	map<int64_t, Task_time> copyOfActiveTasks = active_tasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		Task_time tt = it->second;
		stop(tt.task_ID);
	}
}

void Time_keeper::stop_all_and_continue()
{
	map<int64_t, Task_time>::iterator it;
	map<int64_t, Task_time> copyOfActiveTasks = active_tasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		Task_time tt = it->second;
		stop(tt.task_ID);
		start(tt.task_ID);
	}
}

bool Time_keeper::is_idle()
{
	return is_idle_;
}
int Time_keeper::minutes_idle()
{
	return idle_detector.minutes_idle();
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
