#ifndef TIMEIT_EVENTOBSERVER_H_
#define TIMEIT_EVENTOBSERVER_H_

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


	virtual void on_message(EventType type, const string headline, const string message) const {};

	virtual void on_parent_changed( Task_ID id )    {}; // Parent of task changed
	virtual void on_task_added( Task_ID id )        {};
	virtual void on_task_updated( Task_ID id )      {};
	virtual void on_task_removed( Task_ID id )      {};
	virtual void on_task_name_changed( Task_ID id ) {};
	virtual void on_task_time_changed( Task_ID id ) {}; // Time connected to this task has changed
	virtual void on_complete_update()               {}; // Database have many changes, re populate views
	virtual void on_settings_changed( string name ) {};
	virtual void on_show_main_window()              {}; //IPC request to show main window
	virtual void on_time_entry_changed( Time_ID id) {};
private:
	Notifier& publisher;
};

}
#endif // TIMEIT_EVENTOBSERVER_H_