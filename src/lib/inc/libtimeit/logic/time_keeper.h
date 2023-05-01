
#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H

static const auto minute = 60;

#include <list>
#include <map>
#include <libtimeit/datatypes/task.h>
#include <libtimeit/timer.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include "system_info.h"

namespace libtimeit
{
using namespace std;
class time_manager;

class time_manager_observer
{
public:
	time_manager_observer( time_manager& /*time_keeper*/);
	time_manager_observer( const time_manager_observer& ) = delete;
	time_manager_observer( time_manager_observer&& )      = delete;
	time_manager_observer& operator=( const time_manager_observer& ) = delete;
	time_manager_observer& operator=( time_manager_observer&& )      = delete;
	~time_manager_observer();

	virtual void on_idle_detected(const time_entry& /*id*/) {};
	virtual void on_running_changed()       {};
private:
	time_manager& time_keeper;
};

class time_manager :
		public timer_observer,
		public event_observer
{
	friend class time_manager_observer;
public:
	time_manager(
			database& db,
			timer_base&    timer,
			notification_manager& notifier
			);


	void start(task_id id);
	void stop(task_id id);
	void toggle(task_id id);

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
	void on_time_entry_changed(const time_entry& /*te*/) override;
	void stop_time(const time_entry& item);
private:
	//
	void attach( time_manager_observer *);
	void detach( time_manager_observer *);

	void check_for_status_change();

	void on_task_removed(const task&  /*id*/) override;
	void on_settings_changed(string /*name*/) override;
	void on_complete_update() override;

	seconds idle_gz{0s};
	minutes default_idle_time{0min};

	void notify_running_changed();
	void notify_idle_detected(const time_entry& /*id*/);

	list<time_manager_observer *> observers;

	time_accessor      times;
	task_accessor      tasks;
	settings_accessor  settings;

	system_info        system;
	void update_running_entries();
	void check_if_tasks_should_be_stopped();

	vector<task_id> old_running{};
};
}

#endif /*TIME_KEEPER_H*/
