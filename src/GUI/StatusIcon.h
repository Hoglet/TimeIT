/*
 * StatusIcon.h
 *
 *  Created on: 21-Jun-08
 *      Author: hoglet
 */

#ifndef STATUSICON_H_
#define STATUSICON_H_

#include <gtkmm.h>

#include "TimeKeeper.h"
#include "ExtendedTaskAccessor.h"
#include "TimeAccessor.h"
#include "IActionObserver.h"

namespace GUI
{

class StatusIcon :  public TimekeeperObserver, public IStatusIcon, public DB::TaskAccessorObserver
{
public:
	virtual ~StatusIcon();
	StatusIcon(std::shared_ptr<ITimeKeeper>&, std::shared_ptr<DB::IExtendedTaskAccessor>&, std::shared_ptr<DB::ITimeAccessor>&);
	virtual void show() {}; //Shown directly on creation. Might change
	virtual void attach(IActionObserver* observer);
	virtual void detach(IActionObserver* observer);
private:
	void on_activate();
	void setIcon();
	void setTooltip();
	void on_runningChanged();
	void on_idleDetected(){};
	void on_popup_menu(guint button, guint32 activate_time);
	void toggleMainWindow();
	void on_menu_file_popup_quit();
	void on_menu_file_popup_open();
	void on_menu_stop_all_timers();
	void on_menu_about();
	void on_menu_preferences();
	void on_menu_toggle_task1();
	void on_menu_toggle_task2();
	void on_menu_toggle_task3();
	void on_menu_toggle_task4();
	void on_menu_toggle_task5();
	void populateContextMenu();
	void toggleTask(int64_t id);
	std::string completeTaskPath(int64_t id);

	virtual void on_taskAdded(int64_t)  {};
	virtual void on_taskUpdated(int64_t) ;
	virtual void on_taskRemoved(int64_t) {};
	virtual void on_completeUpdate();
	Glib::RefPtr<Gtk::StatusIcon> m_statusIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_defaultIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> idleIconSmall;
	Glib::RefPtr<Gdk::Pixbuf> runningIconSmall;

	Gtk::Menu m_Menu_Popup;
	std::shared_ptr<ITimeKeeper> m_timekeeper;
	std::shared_ptr<DB::IExtendedTaskAccessor> m_taskaccessor;
	std::shared_ptr<DB::ITimeAccessor> m_timeaccessor;
	std::list<IActionObserver*> observers;
	std::vector<int64_t> latestTasks;

};

}
#endif /* STATUSICON_H_ */
