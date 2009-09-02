/*
 * StatusIcon.h
 *
 *  Created on: 21-Jun-08
 *      Author: hoglet
 */

#ifndef STATUSICON_H_
#define STATUSICON_H_

#include <gtkmm.h>
#include "ITimeKeeper.h"

namespace GUI
{
namespace Internal
{

class StatusIcon :  public TimekeeperObserver, public IStatusIcon
{
public:
	virtual ~StatusIcon();
	StatusIcon(boost::shared_ptr<ITimeKeeper>&);
	virtual void show() {}; //Shown directly on creation. Might change
	virtual void attach(IActionObserver* observer);
	virtual void detach(IActionObserver* observer);
private:
	void on_activate();
	void setIcon();
	void on_runningChanged();
	void on_idleDetected(){};
	void on_popup_menu(guint button, guint32 activate_time);
	void toggleMainWindow();
	void on_menu_file_popup_quit();
	void on_menu_file_popup_open();
	void on_menu_stop_all_timers();
	Glib::RefPtr<Gtk::StatusIcon> m_statusIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_defaultIcon;
	Glib::RefPtr<Gdk::Pixbuf> m_runningIcon;
	Gtk::Menu m_Menu_Popup;
	boost::shared_ptr<ITimeKeeper> m_timekeeper;

	std::list<IActionObserver*> observers;
};

}
}
#endif /* STATUSICON_H_ */
