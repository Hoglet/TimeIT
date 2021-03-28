#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <list>
#include <map>
#include <libtimeit/db/Task.h>
#include <libtimeit/Timer.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/db/Database.h>
#include <libtimeit/logic/X11_IdleDetector.h>
#include <libtimeit/db/TaskAccessor.h>
#include <libtimeit/db/SettingsAccessor.h>
#include <libtimeit/db/TimeAccessor.h>

namespace libtimeit
{
using namespace std;

class TimekeeperObserver
{
public:
	virtual ~TimekeeperObserver();
	virtual void on_idleDetected() = 0;
	virtual void on_activityResumed() = 0;
	virtual void on_runningChanged() = 0;
};

class ITimeKeeper
{
public:
	virtual ~ITimeKeeper();
	virtual void StartTask(int64_t id) = 0;
	virtual void StopTask(int64_t id) = 0;
	virtual void ToggleTask(int64_t id) = 0;

	virtual bool isActiveTask(int64_t taskID) = 0;
	virtual bool hasRunningTasks() = 0;
	virtual bool isIdle() = 0;
	virtual time_t timeIdle() = 0;

	//Enable (or disable) automatic time keeping.
	virtual void enable(bool) = 0;
	//Stop all tasks without saving new time records
	virtual void stopAll() = 0;
	//Stop all tasks, without saving new time records, and then start them again
	virtual void stopAllAndContinue() = 0;

	//
	virtual void attach(TimekeeperObserver *) = 0;
	virtual void detach(TimekeeperObserver *) = 0;
};

class Timekeeper :
		public TimerObserver,
		public ITimeKeeper,
		public EventObserver,
		public SettingsAccessorObserver
{
public:
	Timekeeper(Database &database, Timer &timer);
	virtual ~Timekeeper();

	void StartTask(int64_t id);
	void StopTask(int64_t id);
	void ToggleTask(int64_t id);

	bool isActiveTask(int64_t taskID);
	bool hasRunningTasks();

	//Enable (or disable) automatic time keeping.
	void enable(bool);

	//Stop all tasks without saving new time records
	void stopAll();

	//Stop all tasks, without saving new time records, and then start them again
	void stopAllAndContinue();

	//
	void attach(TimekeeperObserver *);
	void detach(TimekeeperObserver *);

	//
	virtual bool isIdle();
	virtual time_t timeIdle();
	virtual int minutesIdle();

	//TimerProxyObserver interface
	void on_signal_1_second();
	void on_signal_10_seconds();

private:
	void UpdateTask(int64_t id, time_t now);

	virtual void on_taskAdded(int64_t)
	{
	};

	virtual void on_taskUpdated(int64_t)
	{
	};

	virtual void on_taskNameChanged(int64_t)
	{
	};

	virtual void on_taskTimeChanged(int64_t)
	{
	};
	virtual void on_taskRemoved(int64_t);
	virtual void on_settingsChanged(const string &);
	virtual void on_completeUpdate();

	void UpdateTask(int64_t id);

	bool enabled;
	bool is_idle;

	struct TaskTime
	{
		int64_t taskID;
		int64_t dbHandle;
		time_t startTime;
		time_t stopTime; //Latest confirmed point in time
	};
	map<int64_t, TaskTime> active_tasks;

	int idle_Gz;
	int idle_Gt;

	void notifyRunningChanged();
	void notifyIdleDetected();
	void notifyActivityResumed();
	list<TimekeeperObserver *> observers;

	TimeAccessor      time_accessor;
	TaskAccessor      task_accessor;
	Timer            &timer;
	SettingsAccessor  settings_accessor;

	X11_IdleDetector idle_detector;
};
}
#endif /*TIMEKEEPER_H_*/