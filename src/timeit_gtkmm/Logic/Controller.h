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
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/db/extended_task_accessor.h>
#include <MainWindow/Summary.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/database.h>
#include <libtimeit/misc/IpcServer.h>
#include <libtimeit/Event_observer.h>

namespace GUI
{

using namespace std;
using namespace libtimeit;

class Controller :
		public ActionObserver,
		public TimekeeperObserver,
		public SummaryObserver,
		public Event_observer
{
public:
	Controller(
			IGUIFactory &guiFactory,
			ITimeKeeper &timeKeeper,
			Database &database,
			IpcServer &);
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
	virtual void on_showDetailsClicked(GUI::ISummary *summary, int64_t taskId, time_t startTime, time_t stopTime);
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime);

	//
	virtual void on_idleDetected();
	virtual void on_activityResumed();
	virtual void on_idleChanged();
	virtual void on_runningChanged();
	virtual void on_action_toggleMainWindow();
	virtual void on_show_main_window();
	virtual void on_action_revertAndContinue();
	virtual void on_action_revertAndStop();
	virtual void on_action_continue();
	virtual void on_action_stopTimers();

private:
	IGUIFactory &guiFactory;
	ITimeKeeper &timeKeeper;
	shared_ptr<IdleDialog> idleDialog;
	Extended_task_accessor taskAccessor;
	Time_accessor         timeAccessor;
	Settings_accessor     settingsAccessor;

	int mainWindow_x = 0;
	int mainWindow_y = 0;
	int selectedTaskID = -1;
	long idleStartTime = 0;

};
}
#endif /* CONTROLLER_H_ */
