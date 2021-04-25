
#ifndef TIME_KEEPER_H_
#define TIME_KEEPER_H_

static const int MINUTE = 60;

#include <list>
#include <map>
#include <libtimeit/db/task.h>
#include <libtimeit/timer.h>
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
	virtual void on_idle_detected( Time_ID ) = 0;
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

	~Time_keeper() = default;

	void start(Task_ID id);
	void stop(Task_ID id);
	void toggle(Task_ID id);

	bool hasRunningTasks();

	//Stop all tasks without saving new time records
	void stop_all();

	//Stop all tasks, without saving new time records, and then start them again
	void stop_all_and_continue();

	//
	void attach(Time_keeper_observer *);
	void detach(Time_keeper_observer *);

	//
	[[nodiscard]] bool   is_idle() const;
	time_t               time_idle();

	//TimerProxyObserver interface
	void on_signal_10_seconds() override;

private:
	void the_method_that_do_stuff();

	void on_task_removed(Task_ID /*id*/) override;
	void on_settings_changed(string /*name*/) override;
	void on_complete_update() override;

	int idle_Gz;
	unsigned default_idle_time{0};

	void notify_running_changed();
	void notify_idle_detected( Time_ID /*id*/);
	void notify_activity_resumed();

	list<Time_keeper_observer *> observers;

	Time_accessor      time_accessor;
	Task_accessor      task_accessor;
	Settings_accessor  settings_accessor;

	X11_idle_detector idle_detector;
	bool user_is_active();
	void update_running_entries();
	void check_if_tasks_should_be_stopped();
	void stop_time(const int64_t id);
};
}

#endif /*TIME_KEEPER_H_*/
