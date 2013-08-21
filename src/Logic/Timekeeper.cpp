#include <IIdleDialog.h>
#include <Timekeeper.h>
#include <Timer.h>
#include <iostream>
#include <DefaultValues.h>

using namespace std;
//using namespace nsIdleDialog;

//TODO Autostart on movement (if autotracker and supressed dialog)

Timekeeper::Timekeeper(const std::shared_ptr<DB::Database>& database, const std::shared_ptr<Timer>& timer)
{
	m_timer = timer;
	m_timeAccessor = database->getTimeAccessor();
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
	if(name.length() < 1 || name == "Gt")
	{
		m_idleGt = m_settingsAccessor->GetIntByName("Gt", DEFAULT_GT);
	}
	if (name.length()<1 || name == "Gz")
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
			for (it = activeTasks.begin(); it != activeTasks.end(); it++)
			{
				TaskTime tt = it->second;
				UpdateTask(tt.taskID);
			}
		}
		else if (hasRunningTasks())
		{
			if (m_idleDetector.minutesIdle() >= m_idleGt)
			{
				notifyIdleDetected();
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
		task.startTime = time(NULL);
		task.stopTime = time(NULL);
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

		if (activeTasks.size() <= 0)
		{
			notifyRunningChanged();
		}
	}
}

void Timekeeper::on_taskRemoved(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		activeTasks.erase(it);
		if (activeTasks.size() <= 0)
		{
			notifyRunningChanged();
		}
	}
}

void Timekeeper::UpdateTask(int64_t id)
{
	map<int64_t, TaskTime>::iterator it;
	it = activeTasks.find(id);
	if (it != activeTasks.end())
	{
		it->second.stopTime = time(NULL);
		TaskTime task = it->second;

		m_timeAccessor->changeEndTime(task.dbHandle, task.stopTime);
	}
}
bool Timekeeper::hasRunningTasks()
{
	return (activeTasks.size() > 0);
}


void Timekeeper::stopAll()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = activeTasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); it++)
	{
		TaskTime tt = it->second;
		StopTask(tt.taskID);
	}
}

void Timekeeper::stopAllAndContinue()
{
	map<int64_t, TaskTime>::iterator it;
	map<int64_t, TaskTime> copyOfActiveTasks = activeTasks;
	for (it = copyOfActiveTasks.begin(); it != copyOfActiveTasks.end(); it++)
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
	std::list<TimekeeperObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		TimekeeperObserver* observer = *iter;
		observer->on_runningChanged();
	}
}

void Timekeeper::notifyIdleDetected()
{
	std::list<TimekeeperObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		TimekeeperObserver* observer = *iter;
		observer->on_idleDetected();
	}
}

void Timekeeper::enable(bool enable)
{
	m_enabled = enable;
}

void Timekeeper::attach(TimekeeperObserver* observer)
{
	observers.push_back(observer);
}
void Timekeeper::detach(TimekeeperObserver* observer)
{
	observers.remove(observer);
}
