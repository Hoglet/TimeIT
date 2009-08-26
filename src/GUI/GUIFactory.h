/*
 * GUIFactory.h
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#ifndef GUIFACTORY_H_
#define GUIFACTORY_H_



#include <Database.h>
#include <boost/shared_ptr.hpp>

#include <ITimeKeeper.h>
#include <IGUIFactory.h>
#include <Timer.h>
#include <gtkmm.h>

namespace GUI
{

class GUIFactory: public IGUIFactory
{
public:
	GUIFactory(boost::shared_ptr<ITimeKeeper>&, boost::shared_ptr<DB::Database>& database, const boost::shared_ptr<Timer>& timer);
	virtual ~GUIFactory();
	virtual WidgetPtr getWidget(EWidget);
	virtual IStatusIcon& getStatusIcon();
	virtual WidgetPtr getAddTime(int64_t taskID);

	virtual void run();
	static void quit();
	static void init(int argc, char *argv[]);
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

	boost::shared_ptr<ITimeKeeper> timekeeper;

	boost::shared_ptr<DB::Database> database;
	boost::shared_ptr<Timer> timer;
	static boost::shared_ptr<Gtk::Main> main;
};
}
#endif /* GUIFACTORY_H_ */
