#include <libtimeit/Timer.h>
#include <iostream>
#include <libtimeit/db/DefaultValues.h>
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
	auto idle=m_idleDetector.idle();
	// std::cout << "Timekeeper::on_signal_1_second() was m_isIdle " << m_isIdle << " will be idle " << idle << std::endl;
	if(idle!=m_isIdle)
	{
		m_isIdle = idle;
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
		IDatabase& database,
		Timer& timer)
		:
		m_timeAccessor(database.getTimeAccessor()),
		m_timer(timer)

{
	m_timeAccessor->stopAllRunning();
	m_taskAccessor = database.getTaskAccessor();
	m_settingsAccessor = database.getSettingsAccessor();
	m_settingsAccessor->attach(this);
	on_settingsChanged("");
	m_enabled = true;
	m_isIdle = false;
	m_timer.attach(this);
	m_taskAccessor->attach(this);
}

Timekeeper::~Timekeeper()
{
	m_timer.detach(this);
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
		if (m_idleDetector.minutesIdle() < m_idleGz)
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
		task.startTime = libtimeit::now();
		task.stopTime = libtimeit::now();
		task.dbHandle = m_timeAccessor->newTime(id, task.startTime, task.stopTime);
		task.taskID = id;
		activeTasks[id] = task;
		m_timeAccessor->setRunning(task.dbHandle, true);
		notifyRunningChanged();
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
		notifyRunningChanged();
	}
}

void Timekeeper::on_taskRemoved(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		activeTasks.erase(it);
		notifyRunningChanged();
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
		it->second.stopTime = libtimeit::now();
		TaskTime task = it->second;

		auto te = m_timeAccessor->getByID(task.dbHandle);
		if(te)
		{
			m_timeAccessor->update(te->withStop(task.stopTime));
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
	return activeTasks.find(taskID) != activeTasks.end();
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

bool Timekeeper::isIdle()
{
	return m_isIdle;
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
