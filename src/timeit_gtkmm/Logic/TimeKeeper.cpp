#include <IdleDialog.h>
#include <Timer.h>
#include <iostream>
#include <DefaultValues.h>
#include "TimeKeeper.h"
#include <Utils.h>

using namespace std;

TimekeeperObserver::~TimekeeperObserver()
{

}

ITimeKeeper::~ITimeKeeper()
{

}

//LCOV_EXCL_START
void Timekeeper::on_signal_1_second()
{
}
//LCOV_EXCL_STOP

Timekeeper::Timekeeper(const std::shared_ptr<DB::IDatabase>& database, const std::shared_ptr<Timer>& timer) :
		m_timeAccessor(database->getTimeAccessor()),m_timer(timer)

{
	m_timeAccessor->stopAllRunning();
	m_taskAccessor = database->getTaskAccessor();
	m_settingsAccessor = database->getSettingsAccessor();
	m_settingsAccessor->attach(this);
	on_settingsChanged("");
	m_enabled = true;
	m_timer->attach(this);
	m_taskAccessor->attach(this);
}

Timekeeper::~Timekeeper()
{
	m_timer->detach(this);
	m_taskAccessor->detach(this);
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.begin();
	while (it != activeTasks.end())
	{
		StopTask(it->second.taskID);
		it = activeTasks.begin();
	}
}

void Timekeeper::on_settingsChanged(const std::string& name)
{
	if (name.length() < 1 || name == "Gt")
	{
		long idleGt = m_settingsAccessor->GetIntByName("Gt", DEFAULT_GT);
		m_idleDetector.setIdleTimeout(idleGt);
	}
	if (name.length() < 1 || name == "Gz")
	{
		m_idleGz = m_settingsAccessor->GetIntByName("Gz", DEFAULT_GZ);
	}
}

void Timekeeper::on_signal_10_seconds()
{
	if (m_enabled)
	{
		map<int64_t, TaskTime>::iterator it;
		if (m_idleDetector.idle())
		{
			if (hasRunningTasks())
			{
				notifyIdleDetected();
			}
		}
		else if (m_idleDetector.minutesIdle() < m_idleGz)
		{
			//Only saving time when being certain that somebody is working
			for (it = activeTasks.begin(); it != activeTasks.end(); ++it)
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
	it = activeTasks.find(id);
	if (it == activeTasks.end())
	{
		TaskTime task;
		task.startTime = Utils::now();
		task.stopTime = Utils::now();
		task.dbHandle = m_timeAccessor->newTime(id, task.startTime, task.stopTime);
		task.taskID = id;
		activeTasks[id] = task;
		m_timeAccessor->setRunning(task.dbHandle, true);
		if (activeTasks.size() == 1)
		{
			notifyRunningChanged();
		}
	}
}

void Timekeeper::ToggleTask(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it == activeTasks.end())
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
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		TaskTime task = it->second;
		activeTasks.erase(it);
		m_timeAccessor->setRunning(task.dbHandle, false);

		if (activeTasks.empty() == true)
		{
			notifyRunningChanged();
		}
	}
	m_idleDetector.setEnabled(activeTasks.empty() == false);
}

void Timekeeper::on_taskRemoved(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		activeTasks.erase(it);
		if (activeTasks.empty())
		{
			notifyRunningChanged();
		}
	}
}
void Timekeeper::on_completeUpdate()
{
	//TODO Detect task removal during syncing
}

void Timekeeper::UpdateTask(int64_t id, time_t now)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		it->second.stopTime = Utils::now();
		TaskTime task = it->second;

		DB::TimeEntry te = m_timeAccessor->getByID(task.dbHandle);
		m_timeAccessor->update(te.withStop(task.stopTime));
	}
}

void Timekeeper::UpdateTask(int64_t id)
{
	time_t now = Utils::now();
	UpdateTask(id, now);
}
bool Timekeeper::hasRunningTasks()
{
	return (activeTasks.empty() == false);
}

void Timekeeper::stopAll()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = activeTasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		TaskTime tt = it->second;
		StopTask(tt.taskID);
	}
}

void Timekeeper::stopAllAndContinue()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = activeTasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); ++it)
	{
		TaskTime tt = it->second;
		StopTask(tt.taskID);
		StartTask(tt.taskID);
	}
}

int Timekeeper::minutesIdle()
{
	return m_idleDetector.minutesIdle();
}
time_t Timekeeper::timeIdle()
{
	return m_idleDetector.timeIdle();
}

void Timekeeper::notifyRunningChanged()
{
	m_idleDetector.setEnabled(activeTasks.empty() == false);
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

void Timekeeper::enable(bool enable)
{
	m_enabled = enable;
	if (m_enabled)
	{
		m_idleDetector.reset();
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
