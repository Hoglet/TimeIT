
#ifndef TIME_KEEPER_H_
#define TIME_KEEPER_H_

static const int MINUTE = 60;

#include <list>
#include <map>
#include <libtimeit/db/task.h>
#include <libtimeit/timer.h>
#include <libtimeit/db/database.h>
#include <libtimeit/logic/x11_idle_detector.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/time_accessor.h>

namespace libtimeit
{
using namespace std;
class Time_keeper;

class time_keeper_observer
{
public:
	time_keeper_observer(Time_keeper& /*time_keeper*/);
	time_keeper_observer(const time_keeper_observer& ) = delete;
	time_keeper_observer(time_keeper_observer&& )      = delete;
	time_keeper_observer& operator=(const time_keeper_observer& ) = delete;
	time_keeper_observer& operator=(time_keeper_observer&& )      = delete;
	~time_keeper_observer();

	virtual void on_idle_detected(const Time_entry& /*id*/) {};
	virtual void on_running_changed()       {};
private:
	Time_keeper& time_keeper;
};

class Time_keeper :
		public timer_observer,
		public event_observer
{
	friend class time_keeper_observer;
public:
	Time_keeper(
			database& db,
			Timer&    timer,
			notification_manager& notifier
			);


	void start(Task_id id);
	void stop(Task_id id);
	void toggle(Task_id id);

	bool has_running_tasks();
	bool user_is_active();

	//Stop all tasks without saving new time records
	void stop_all();

	//Stop all tasks, without saving new time records, and then start them again


	//
	[[nodiscard]] bool   tasks_are_running() const;
	[[maybe_unused]] [[nodiscard]] bool   is_idle();
	//TimerProxyObserver interface
	void on_signal_10_seconds() override;

	//Time_observer
	void on_time_entry_changed(const Time_entry& /*te*/) override;
	void stop_time(const Time_entry& te);
private:
	//
	void attach(time_keeper_observer *);
	void detach(time_keeper_observer *);

	void check_for_status_change();

	void on_task_removed(Task_id /*id*/) override;
	void on_settings_changed(string /*name*/) override;
	void on_complete_update() override;

	int idle_gz;
	unsigned default_idle_time{0};

	void notify_running_changed();
	void notify_idle_detected(const Time_entry& /*id*/);

	list<time_keeper_observer *> observers;

	time_accessor      times;
	task_accessor      tasks;
	settings_accessor  settings;

	X11_idle_detector idle_detector;
	void update_running_entries();
	void check_if_tasks_should_be_stopped();

	vector<Task_id> old_running{};
};
}

#endif /*TIME_KEEPER_H_*/
