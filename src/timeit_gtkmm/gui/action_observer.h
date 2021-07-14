/*
 * IActionObserver.h
 *
 *  Created on: 2008-dec-21
 *      Author: hoglet
 */

#ifndef ACTION_OBSERVER_H_
#define ACTION_OBSERVER_H_

#include <libtimeit/db/data_types.h>
namespace gui
{

using namespace libtimeit;

class action_observer
{
public:
	virtual void on_action_task_selection_changed(Task_id /*selectedTaskID*/) {};
	virtual void on_action_add_task()           {};
	virtual void on_action_remove_task()        {};
	virtual void on_action_add_time()           {};
	virtual void on_action_start_task()         {};
	virtual void on_action_stop_task()          {};
	virtual void on_action_edit_task()          {};
	virtual void on_action_about()              {};
	virtual void on_action_help()               {};
	virtual void on_action_quit()             	{};
	virtual void on_action_preferences()        {};
	virtual void on_action_toggle_main_window() {};
	virtual void on_action_show_main_window()   {};
	virtual void on_action_stop_timers()         {};
	virtual void on_action_report_bug()         {};
};
}
#endif
