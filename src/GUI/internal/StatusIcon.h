/*
 * StatusIcon.h
 *
 *  Created on: 21-Jun-08
 *      Author: hoglet
 */

#ifndef STATUSICON_H_
#define STATUSICON_H_

#include <gtkmm.h>
#include <ITimeKeeper.h>
#include <ITaskAccessor.h>
#include <ITimeAccessor.h>

namespace GUI
{
namespace Internal
{

class StatusIcon :  public TimekeeperObserver, public IStatusIcon, public TaskAccessorObserver
{
public:
	virtual ~StatusIcon();
	StatusIcon(boost::shared_ptr<ITimeKeeper>&, boost::shared_ptr<ITaskAccessor>&, boost::shared_ptr<ITimeAccessor>&);
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
	void on_menu_toggle_task1();
	void on_menu_toggle_task2();
	void on_menu_toggle_task3();
	void on_menu_toggle_task4();
	void on_menu_toggle_task5();
	void populateContextMenu();
	void toggleTask(int64_t id);
	std::string completeTaskPath(int64_t id);

	virtual void on_taskAdded(const Task&)  {};
	virtual void on_taskUpdated(const Task&) ;
	virtual void on_taskRemoved(int64_t) {};

	Glib::RefPtr<Gtk::StatusIcon> m_statusIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_defaultIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> idleIconSmall;
	Glib::RefPtr<Gdk::Pixbuf> runningIconSmall;

	Gtk::Menu m_Menu_Popup;
	boost::shared_ptr<ITimeKeeper> m_timekeeper;
	boost::shared_ptr<ITaskAccessor> m_taskaccessor;
	boost::shared_ptr<ITimeAccessor> m_timeaccessor;
	std::list<IActionObserver*> observers;
	std::vector<int64_t> latestTasks;

};

}
}
#endif /* STATUSICON_H_ */
