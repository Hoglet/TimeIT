#ifndef EVENT_OBSERVER_H
#define EVENT_OBSERVER_H

#include <cstdint>
#include <string>
#include <libtimeit/datatypes/time_entry.h>
#include <libtimeit/datatypes/task.h>

namespace libtimeit
{
using namespace std;

enum class EventType
{
	ERROR_MESSAGE
	//INFO_MESSAGE
};

class notification_manager;

class event_observer
{
public:
	event_observer(notification_manager&);
	virtual ~event_observer();
	event_observer(const event_observer&) = delete;
	event_observer(event_observer&&) = delete;
	event_observer& operator=(const event_observer&) = delete;
	event_observer& operator=(event_observer&&) = delete;

	virtual void on_message(EventType /*type*/, string /*headline*/, string /*message*/) const {};

	virtual void on_parent_changed(const task&  /*id*/ )       {}; // Parent of task changed
	virtual void on_task_added(const task&  /*id*/ )           {};
	virtual void on_task_updated(const task_id&  /*id*/ )      {};
	virtual void on_task_removed(const task& /*id*/ )         {};
	virtual void on_task_name_changed(const task&  /*id*/ )    {};
	virtual void on_task_time_changed(const task_id&  /*id*/ )    {}; // Time connected to this task has changed
	virtual void on_complete_update()                     {}; // Database have many changes, re populate views
	virtual void on_settings_changed( string /*name*/ )   {};
	virtual void on_show_main_window()                    {}; //IPC request to show main window
	virtual void on_time_entry_changed(const time_entry& /*te*/) {};
private:
	notification_manager& publisher;
};

}
#endif // EVENT_OBSERVER_H