/*
 * Controller.h
 *
 *  Created on: Mar 12, 2009
 *      Author: hoglet
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <IActionObserver.h>
#include <IGUIFactory.h>
#include <IdleDialog.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/db/ExtendedTaskAccessor.h>
#include <MainWindow/Summary.h>
#include <libtimeit/db/ISettingsAccessor.h>
#include <libtimeit/db/Database.h>
#include <libtimeit/misc/IpcServer.h>
#include <libtimeit/EventObserver.h>

namespace GUI
{

using namespace std;
using namespace libtimeit;

class Controller :
		public IActionObserver,
		public TimekeeperObserver,
		public SummaryObserver,
		public EventObserver
{
public:
	Controller(
			IGUIFactory &guiFactory,
			ITimeKeeper &timeKeeper,
			IDatabase &database,
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
	virtual void on_action_showMainWindow();
	virtual void on_action_revertAndContinue();
	virtual void on_action_revertAndStop();
	virtual void on_action_continue();
	virtual void on_action_stopTimers();

private:
	IGUIFactory &guiFactory;
	ITimeKeeper &timeKeeper;
	shared_ptr<IdleDialog> idleDialog;
	shared_ptr<IExtendedTaskAccessor> taskAccessor;
	shared_ptr<ITimeAccessor> timeAccessor;
	shared_ptr<ISettingsAccessor> settingsAccessor;

	int mainWindow_x = 0;
	int mainWindow_y = 0;
	int selectedTaskID = -1;
	long idleStartTime = 0;

};
}
#endif /* CONTROLLER_H_ */
