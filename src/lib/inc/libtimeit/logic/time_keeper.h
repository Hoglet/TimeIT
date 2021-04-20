#ifndef TIME_KEEPER_H_
#define TIME_KEEPER_H_

static const int MINUTE = 60;

#include <list>
#include <map>
#include <libtimeit/db/task.h>
#include <libtimeit/timer.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/database.h>
#include <libtimeit/logic/X11_idle_detector.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/time_accessor.h>

namespace libtimeit
{
using namespace std;

class Time_keeper_observer
{
public:
	virtual ~Time_keeper_observer() = default;
	virtual void on_idle_detected() = 0;
	virtual void on_activity_resumed() = 0;
	virtual void on_running_changed() = 0;
};

class Time_keeper :
		public Timer_observer,
		public Event_observer
{
public:
	Time_keeper(
			Database& database,
			Timer&    timer,
			Notifier& notifier
			);

	virtual ~Time_keeper();

	void start(Task_ID id);
	void stop(Task_ID id);
	void toggle(Task_ID id);

	bool is_active_task(int64_t taskID);
	bool hasRunningTasks();

	//Enable (or disable) automatic time keeping.
	void enable(bool);

	//Stop all tasks without saving new time records
	void stop_all();

	//Stop all tasks, without saving new time records, and then start them again
	void stop_all_and_continue();

	//
	void attach(Time_keeper_observer *);
	void detach(Time_keeper_observer *);

	//
	virtual bool   is_idle();
	virtual time_t time_idle();
	virtual int    minutes_idle();

	//TimerProxyObserver interface
	void on_signal_1_second();
	void on_signal_10_seconds();

private:

	virtual void on_task_added(Task_ID /*id*/)
	{
	};

	virtual void on_task_updated(Task_ID /*id*/)
	{
	};

	virtual void on_task_name_changed(Task_ID /*id*/)
	{
	};

	virtual void on_task_time_changed(Task_ID /*id*/)
	{
	};
	virtual void on_task_removed(Task_ID /*id*/);
	virtual void on_settings_changed(string);
	virtual void on_complete_update();

	void update_task(int64_t id);

	bool enabled{true};
	bool is_idle_{false};

	struct Task_time
	{
		Task_ID task_ID;
		Time_ID time_ID;
		time_t  start;
		time_t  stop; //Latest confirmed point in time
	};
	map<int64_t, Task_time> active_tasks;

	int idle_Gz;
	int idle_Gt;

	void notify_running_changed();
	void notify_idle_detected();
	void notify_activity_resumed();

	list<Time_keeper_observer *> observers;

	Time_accessor      time_accessor;
	Settings_accessor  settings_accessor;

	X11_idle_detector idle_detector;
};
}
#endif /*TIME_KEEPER_H_*/
