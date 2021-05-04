#ifndef TIMEIT_EVENT_OBSERVER_H_
#define TIMEIT_EVENT_OBSERVER_H_

#include <cstdint>
#include <string>
#include <libtimeit/db/data_types.h>

namespace libtimeit
{
using namespace std;

enum class EventType
{
	ERROR_MESSAGE,
	INFO_MESSAGE
};

class Notifier;

class Event_observer
{
public:
	Event_observer(Notifier&);
	virtual ~Event_observer();
	Event_observer(const Event_observer&) = delete;
	Event_observer(Event_observer&&) = delete;
	Event_observer& operator=(const Event_observer&) = delete;
	Event_observer& operator=(Event_observer&&) = delete;

	virtual void on_message(EventType type, string headline, string message) const {};

	virtual void on_parent_changed(Task_id id )    {}; // Parent of task changed
	virtual void on_task_added(Task_id id )        {};
	virtual void on_task_updated(Task_id id )      {};
	virtual void on_task_removed(Task_id id )      {};
	virtual void on_task_name_changed(Task_id id ) {};
	virtual void on_task_time_changed(Task_id id ) {}; // Time connected to this task has changed
	virtual void on_complete_update()               {}; // Database have many changes, re populate views
	virtual void on_settings_changed( string name ) {};
	virtual void on_show_main_window()              {}; //IPC request to show main window
	virtual void on_time_entry_changed(Time_id id) {};
private:
	Notifier& publisher;
};

}
#endif // TIMEIT_EVENT_OBSERVER_H_