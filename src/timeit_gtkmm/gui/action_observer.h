/*
 * IActionObserver.h
 *
 *  Created on: 2008-dec-21
 *      Author: hoglet
 */

#ifndef ACTION_OBSERVER_H_
#define ACTION_OBSERVER_H_

class action_observer
{
public:
	virtual void on_action_task_selection_changed(int selectedTaskID)=0;
	virtual void on_action_add_task()=0;
	virtual void on_action_remove_task()=0;
	virtual void on_action_add_time()=0;
	virtual void on_action_start_task()=0;
	virtual void on_action_stop_task()=0;
	virtual void on_action_edit_task()=0;
	virtual void on_action_about()=0;
	virtual void on_action_help()=0;
	virtual void on_action_quit()=0;
	virtual void on_action_preferences()=0;
	virtual void on_action_toggleMainWindow() {};
	virtual void on_action_showMainWindow() {};
	virtual void on_action_stopTimers() {};
	virtual void on_action_report_bug() {};
};

#endif
