#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <list>
#include <Task.h>
#include <Timer.h>
#include <ITimeKeeper.h>
#include <Database.h>
#include <IdleDetector.h>
#include <boost/weak_ptr.hpp>


class Timekeeper : public TimerObserver, public ITimeKeeper, public TaskAccessorObserver, public ISettingsAccessorObserver
{
public:
	Timekeeper(const boost::shared_ptr<DB::Database>& database, const boost::shared_ptr<Timer>& timer);
	virtual ~Timekeeper();

	void StartTask(int64_t id);
	void StopTask(int64_t id);

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
	virtual time_t timeIdle() ;
	virtual int minutesIdle();
private:

	//TimerProxyObserver interface
	void on_signal_1_second() {};
	void on_signal_10_seconds();

	virtual void on_taskAdded(const Task&)  {};
	virtual void on_taskUpdated(const Task&) {};
	virtual void on_taskRemoved(int64_t);
	virtual void on_settingsChanged( const std::string& );

	void UpdateTask(int64_t id);
	void testfunc();


	GUI::WidgetPtr idleDialog;
	bool m_enabled;

	struct TaskTime
	{
		int64_t taskID;
		int64_t dbHandle;
		time_t  startTime;
		time_t  stopTime; //Latest confirmed point in time
	};
	std::map<int64_t,TaskTime> activeTasks;

	int m_idleGz;
	int m_idleGt;

	void notifyRunningChanged();
	void notifyIdleDetected();
	std::list<TimekeeperObserver*> observers;

	boost::shared_ptr<ITimeAccessor> m_timeAccessor;
	boost::shared_ptr<ITaskAccessor> m_taskAccessor;
	boost::shared_ptr<Timer> m_timer;
	boost::shared_ptr<ISettingsAccessor> m_settingsAccessor;

	IdleDetector m_idleDetector;
};

#endif /*TIMEKEEPER_H_*/
