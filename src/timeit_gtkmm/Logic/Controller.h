/*
 * Controller.h
 *
 *  Created on: Mar 12, 2009
 *      Author: hoglet
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <ActionObserver.h>
#include <IGUIFactory.h>
#include <IdleDialog.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>
#include <MainWindow/Summary.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/database.h>
#include <libtimeit/misc/ipc_server.h>
#include <libtimeit/event_observer.h>

namespace GUI
{

using namespace std;
using namespace libtimeit;

class Controller :
		public ActionObserver,
		public Time_keeper_observer,
		public SummaryObserver,
		public Event_observer
{
public:
	Controller(
			IGUIFactory &guiFactory,
			Time_keeper &timeKeeper,
			Database    &database,
			IpcServer   &ipc_server,
			Notifier    &notifier);
	virtual ~Controller();
	void start();

	//Action observers
	virtual void on_action_task_selection_changed(int selectedTaskID);
	virtual void on_action_add_task();
	virtual void on_action_remove_task();
	virtual void on_action_add_time();
	virtual void on_action_start_task();
	virtual void on_action_stop_task();
	virtual void on_action_edit_task();
	virtual void on_action_about();
	virtual void on_action_help();
	virtual void on_action_quit();
	virtual void on_action_preferences();
	virtual void on_action_report_bug();

	//SummaryObserver
	virtual void on_showDetailsClicked(int64_t taskId, time_t startTime, time_t stopTime);
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime);

	//
	virtual void on_idle_detected();
	virtual void on_activity_resumed();
	virtual void on_idleChanged();
	virtual void on_running_changed();
	virtual void on_action_toggleMainWindow();
	virtual void on_show_main_window();
	virtual void on_action_revertAndContinue();
	virtual void on_action_revertAndStop();
	virtual void on_action_continue();
	virtual void on_action_stopTimers();

private:
	IGUIFactory &guiFactory;
	Time_keeper &timeKeeper;
	shared_ptr<IdleDialog> idleDialog;
	Time_accessor         timeAccessor;
	Settings_accessor     settingsAccessor;

	int mainWindow_x = 0;
	int mainWindow_y = 0;
	int selectedTaskID = -1;
	long idleStartTime = 0;

};
}
#endif /* CONTROLLER_H_ */
