/*
 * StatusIcon.cpp
 *
 *  Created on: 21-Jun-08
 *      Author: hoglet
 */

#include "StatusIcon.h"
#include "MainWindow.h"
#include "Utils.h"
#include <iostream>
#include <glibmm/i18n.h>


namespace GUI
{
namespace Internal
{
using namespace std;

StatusIcon::StatusIcon(boost::shared_ptr<ITimeKeeper>& timekeeper): m_timekeeper(timekeeper)
{
	const std::string& imagePath=Utils::getImagePath();
	std::string defaultIconPath = Glib::build_filename(imagePath,"icon.svg" );
	std::string runningIconPath = Glib::build_filename(imagePath,"running.svg" );
	const char * str = runningIconPath.c_str();
	str = defaultIconPath.c_str();
	m_defaultIcon=Gdk::Pixbuf::create_from_file(defaultIconPath);
	m_runningIcon=Gdk::Pixbuf::create_from_file(runningIconPath);
	m_statusIcon = Gtk::StatusIcon::create(m_defaultIcon);
	setIcon();

	Gtk::Window::set_default_icon( m_defaultIcon );

	m_timekeeper->attach(this);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();

		menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Toggle main window"),
		  sigc::mem_fun(*this, &StatusIcon::on_menu_file_popup_open) ) );
		menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Stop all timers"),
		  sigc::mem_fun(*this, &StatusIcon::on_menu_stop_all_timers) ) );
		menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Quit"),
		  sigc::mem_fun(*this, &StatusIcon::on_menu_file_popup_quit) ) );
	}
    m_statusIcon->set_tooltip(_("TimeIt. the unobtrusive time tracker"));
    m_statusIcon->signal_activate().connect(sigc::mem_fun(this,  &StatusIcon::on_activate) );
	m_statusIcon->signal_popup_menu().connect(sigc::mem_fun(this,  &StatusIcon::on_popup_menu) );
}

StatusIcon::~StatusIcon()
{
	m_timekeeper->detach(this);
}

void StatusIcon::on_activate()
{
	toggleMainWindow();
}
void StatusIcon::toggleMainWindow()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_toggleMainWindow();
	}
}

void StatusIcon::attach(IActionObserver* observer)
{
	observers.push_back(observer);
}
void StatusIcon::detach(IActionObserver* observer)
{
	observers.remove(observer);
}



void StatusIcon::on_menu_file_popup_open()
{
	toggleMainWindow();
}
void StatusIcon::on_menu_file_popup_quit()
{
	Gtk::Main::quit();
}
void StatusIcon::on_menu_stop_all_timers()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		IActionObserver* observer = *iter;
		observer->on_action_stopTimers();
	}
}

void StatusIcon::on_popup_menu(guint button, guint32 activate_time)
{
	m_Menu_Popup.popup(button, activate_time);
}

void StatusIcon::on_runningChanged()
{
	setIcon();
}

void StatusIcon::setIcon()
{
	if(m_timekeeper->hasRunningTasks())
	{
		m_statusIcon->set(m_runningIcon);
     	Gtk::Window::set_default_icon( m_runningIcon );
	}
	else
	{
		m_statusIcon->set(m_defaultIcon);
		Gtk::Window::set_default_icon( m_defaultIcon );
	}

}
}
}
