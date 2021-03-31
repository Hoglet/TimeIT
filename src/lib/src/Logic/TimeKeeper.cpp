#include <libtimeit/Timer.h>
#include <iostream>
#include <libtimeit/db/default_values.h>
#include "libtimeit/logic/TimeKeeper.h"
#include <libtimeit/Utils.h>

using namespace std;
using namespace libtimeit;

TimekeeperObserver::~TimekeeperObserver()
{

}

ITimeKeeper::~ITimeKeeper()
{

}

//LCOV_EXCL_START
void Timekeeper::on_signal_1_second()
{
	auto idle=idle_detector.idle();
	// std::cout << "Timekeeper::on_signal_1_second() was m_isIdle " << m_isIdle << " will be idle " << idle << std::endl;
	if(idle!=is_idle)
	{
		is_idle = idle;
		if(idle)
		{
			notifyIdleDetected();
		}
		else
		{
			notifyActivityResumed();
		}
	}
}
//LCOV_EXCL_STOP

Timekeeper::Timekeeper(
		Database& database,
		Timer& op_timer,
		Notifier& notifier)
		:
		time_accessor( database ),
		task_accessor(database ),
		settings_accessor( database ),
		timer( op_timer ),
		Event_observer(notifier)

{
	time_accessor.stop_all();

	settings_accessor;
	settings_accessor.attach(this);
	on_settings_changed("");
	enabled = true;
	is_idle = false;
	timer.attach(this);
}

Timekeeper::~Timekeeper()
{
	timer.detach(this);
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.begin();
	while (it != active_tasks.end())
	{
		StopTask(it->second.taskID);
		it = active_tasks.begin();
	}
}

void Timekeeper::on_settings_changed(string name)
{
	if (name.length() < 1 || name == "Gt")
	{
		long idleGt = settings_accessor.get_int("Gt", DEFAULT_GT);
		idle_detector.setIdleTimeout(idleGt);
	}
	if (name.length() < 1 || name == "Gz")
	{
		idle_Gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
	}
}

void Timekeeper::on_signal_10_seconds()
{
	if (enabled)
	{
		map<int64_t, TaskTime>::iterator it;
		if (idle_detector.minutesIdle() < idle_Gz)
		{
			//Only saving time when being certain that somebody is working
			for (it = active_tasks.begin(); it != active_tasks.end(); ++it)
			{
				TaskTime tt = it->second;
				UpdateTask(tt.taskID);
			}
		}
	}
}

void Timekeeper::StartTask(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.find(id);
	if (it == active_tasks.end())
	{
		TaskTime task;
		task.startTime = libtimeit::now();
		task.stopTime = libtimeit::now();
		task.dbHandle = time_accessor.create(id, task.startTime, task.stopTime);
		task.taskID = id;
		active_tasks[id] = task;
		time_accessor.setRunning(task.dbHandle, true);
		notifyRunningChanged();
	}
}

void Timekeeper::ToggleTask(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.find(id);
	if (it == active_tasks.end())
	{
		StartTask(id);
	}
	else
	{
		StopTask(id);
	}
}

void Timekeeper::StopTask(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		TaskTime task = it->second;
		active_tasks.erase(it);
		time_accessor.setRunning(task.dbHandle, false);
		notifyRunningChanged();
	}
}

void Timekeeper::on_task_removed(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		active_tasks.erase(it);
		notifyRunningChanged();
	}
}
void Timekeeper::on_complete_update()
{
	//TODO Detect task removal during syncing
}

void Timekeeper::UpdateTask(int64_t id, time_t now)
{
	map<int64_t, TaskTime>::iterator it;
	it = active_tasks.find(id);
	if (it != active_tasks.end())
	{
		it->second.stopTime = libtimeit::now();
		TaskTime task = it->second;

		auto te = time_accessor.by_ID(task.dbHandle);
		if(te)
		{
			time_accessor.update(te->with_stop(task.stopTime));
		}
	}
}

void Timekeeper::UpdateTask(int64_t id)
{
	time_t now = libtimeit::now();
	UpdateTask(id, now);
}

bool Timekeeper::isActiveTask(int64_t taskID)
{
	return active_tasks.find(taskID) != active_tasks.end();
}
bool Timekeeper::hasRunningTasks()
{
	return (active_tasks.empty() == false);
}

void Timekeeper::stopAll()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = active_tasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		TaskTime tt = it->second;
		StopTask(tt.taskID);
	}
}

void Timekeeper::stopAllAndContinue()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = active_tasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		TaskTime tt = it->second;
		StopTask(tt.taskID);
		StartTask(tt.taskID);
	}
}

bool Timekeeper::isIdle()
{
	return is_idle;
}
int Timekeeper::minutesIdle()
{
	return idle_detector.minutesIdle();
}
time_t Timekeeper::timeIdle()
{
	return idle_detector.timeIdle();
}

void Timekeeper::notifyRunningChanged()
{
	std::list<TimekeeperObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		TimekeeperObserver* observer = *iter;
		observer->on_runningChanged();
	}
}

void Timekeeper::notifyIdleDetected()
{
	std::list<TimekeeperObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		TimekeeperObserver* observer = *iter;
		observer->on_idleDetected();
	}
}

void Timekeeper::notifyActivityResumed()
{
	std::list<TimekeeperObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		TimekeeperObserver* observer = *iter;
		observer->on_activityResumed();
	}

}

void Timekeeper::enable(bool enable)
{
	enabled = enable;
	if (enabled)
	{
		idle_detector.reset();
	}
}

void Timekeeper::attach(TimekeeperObserver* observer)
{
	observers.push_back(observer);
}
void Timekeeper::detach(TimekeeperObserver* observer)
{
	observers.remove(observer);
}
