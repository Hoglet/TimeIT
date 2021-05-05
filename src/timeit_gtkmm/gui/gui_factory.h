/*
 * GUIFactory.h
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#ifndef GUI_FACTORY_H_
#define GUI_FACTORY_H_

#include <libtimeit/db/database.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/timer.h>
#include <gtkmm.h>
#include "IWidget.h"
#include "status_icon.h"

namespace gui
{
using namespace libtimeit;
using namespace std;

enum EWidget
{
	MAIN_WINDOW,
	ADD_TASK_DIALOG,
	ABOUT_DIALOG,
	EDIT_TASK_DIALOG,
	ADD_TIME,
	IDLE_DIALOG,
	DETAILS_DIALOG,
	PREFERENCE_DIALOG,
	MAX_WIDGETS
};

using WidgetPtr = std::shared_ptr<IWidget>;

class GUIFactory
{
public:
	GUIFactory(Time_keeper&, Database &database, Timer& timer, Notifier& notifier);
	virtual ~GUIFactory();
	virtual WidgetPtr getWidget(EWidget);
	virtual StatusIcon& getStatusIcon();
	virtual WidgetPtr getAddTime(int64_t taskID);

	void quit();
private:
	WidgetPtr addTimeInstance;
	WidgetPtr addTaskInstance;
	WidgetPtr aboutDialogInstance;
	WidgetPtr mainWindow;
	WidgetPtr editTaskDialogInstace;
	WidgetPtr addTimeDialogInstace;
	WidgetPtr idleDialogInstance;
	WidgetPtr detailsDialogInstance;
	WidgetPtr preferenceDialogInstance;

	void on_addTime_response(int);
	void on_mainWindow_hide();
	void on_addTaskDialog_hide();
	void on_aboutDialog_response(int);
	void on_editTask_hide();
	void on_dialog_hide(const WidgetPtr ptr);
	void on_detailsDialog_hide();
	void on_preferenceDialog_hide();

	Time_keeper& timeKeeper;

	Database& database;
	Timer& timer;
	Notifier& notifier;
	list<WidgetPtr> dialogs;
};
}
#endif /* GUI_FACTORY_H_ */
