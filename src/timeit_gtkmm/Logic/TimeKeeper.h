#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <list>
#include <libtimeit/db/Task.h>
#include <libtimeit/Timer.h>
#include <TimeKeeper.h>
#include <libtimeit/db/Database.h>
#include <IdleDetector.h>
#include <libtimeit/db/TaskAccessor.h>
#include <IGUIFactory.h>

class TimekeeperObserver
{
public:
	virtual ~TimekeeperObserver();
	virtual void on_idleDetected() = 0;
	virtual void on_runningChanged() = 0;
};

class ITimeKeeper
{
public:
	virtual ~ITimeKeeper();
	virtual void StartTask(int64_t id) = 0;
	virtual void StopTask(int64_t id) = 0;
	virtual void ToggleTask(int64_t id) = 0;

	virtual bool hasRunningTasks() = 0;
	virtual time_t timeIdle() = 0;

	//Enable (or disable) automatic time keeping.
	virtual void enable(bool) = 0;
	//Stop all tasks without saving new time records
	virtual void stopAll() = 0;
	//Stop all tasks, without saving new time records, and then start them again
	virtual void stopAllAndContinue() = 0;

	//
	virtual void attach(TimekeeperObserver*) = 0;
	virtual void detach(TimekeeperObserver*) = 0;
};

class Timekeeper: public libtimeit::TimerObserver, public ITimeKeeper, public libtimeit::EventObserver, public libtimeit::ISettingsAccessorObserver
{
public:
	Timekeeper(const std::shared_ptr<libtimeit::IDatabase> &database, libtimeit::Timer &timer);
	virtual ~Timekeeper();

	void StartTask(int64_t id);
	void StopTask(int64_t id);
	void ToggleTask(int64_t id);

	bool hasRunningTasks();

	//Enable (or disable) automatic time keeping.
	void enable(bool);

	//Stop all tasks without saving new time records
	void stopAll();

	//Stop all tasks, without saving new time records, and then start them again
	void stopAllAndContinue();

	//
	void attach(TimekeeperObserver*);
	void detach(TimekeeperObserver*);

	//
	virtual time_t timeIdle();
	virtual int minutesIdle();

	//TimerProxyObserver interface
	void on_signal_1_second();
	void on_signal_10_seconds();

private:
	void UpdateTask(int64_t id, time_t now);

	virtual void on_taskAdded(int64_t)
	{
	}
	;
	virtual void on_taskUpdated(int64_t)
	{
	}
	;
	virtual void on_taskNameChanged(int64_t)
	{
	}
	;
	virtual void on_taskTimeChanged(int64_t)
	{
	}
	;
	virtual void on_taskRemoved(int64_t);
	virtual void on_settingsChanged(const std::string&);
	virtual void on_completeUpdate();

	void UpdateTask(int64_t id);
	void testfunc();

	GUI::WidgetPtr idleDialog;
	bool m_enabled;

	struct TaskTime
	{
		int64_t taskID;
		int64_t dbHandle;
		time_t startTime;
		time_t stopTime; //Latest confirmed point in time
	};
	std::map<int64_t, TaskTime> activeTasks;

	int m_idleGz;
	int m_idleGt;

	void notifyRunningChanged();
	void notifyIdleDetected();
	std::list<TimekeeperObserver*> observers;

	std::shared_ptr<libtimeit::ITimeAccessor> m_timeAccessor;
	std::shared_ptr<libtimeit::ITaskAccessor> m_taskAccessor;
	libtimeit::Timer& m_timer;
	std::shared_ptr<libtimeit::ISettingsAccessor> m_settingsAccessor;

	IdleDetector m_idleDetector;
};

#endif /*TIMEKEEPER_H_*/
