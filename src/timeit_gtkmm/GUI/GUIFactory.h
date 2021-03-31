/*
 * GUIFactory.h
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#ifndef GUIFACTORY_H_
#define GUIFACTORY_H_

#include <libtimeit/db/database.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <IGUIFactory.h>
#include <libtimeit/Timer.h>
#include <gtkmm.h>

namespace GUI
{
using namespace libtimeit;
using namespace std;

class GUIFactory: public IGUIFactory
{
public:
	GUIFactory(ITimeKeeper&, Database &database, Timer& timer, Notifier& notifier);
	virtual ~GUIFactory();
	virtual WidgetPtr getWidget(EWidget);
	virtual IStatusIcon& getStatusIcon();
	virtual WidgetPtr getAddTime(int64_t taskID);

	void quit();
	void init(int argc, char *argv[]);
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
	void on_idleDialog_hide();
	void on_detailsDialog_hide();
	void on_preferenceDialog_hide();

	ITimeKeeper& timeKeeper;

	Database& database;
	Timer& timer;
	Notifier& notifier;
};
}
#endif /* GUIFACTORY_H_ */
