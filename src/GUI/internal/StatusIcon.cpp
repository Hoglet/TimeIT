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
#include <iomanip>

namespace GUI
{
namespace Internal
{
using namespace std;

StatusIcon::StatusIcon(boost::shared_ptr<ITimeKeeper>& timekeeper, boost::shared_ptr<ITaskAccessor>& taskaccessor,
		boost::shared_ptr<ITimeAccessor>& timeaccessor)
{
	m_timekeeper = timekeeper;
	m_taskaccessor = taskaccessor;
	m_timeaccessor = timeaccessor;
	const std::string& imagePath = Utils::getImagePath();
	std::string defaultIconPath = Glib::build_filename(imagePath, "icon.svg");
	std::string runningIconPath = Glib::build_filename(imagePath, "running.svg");
	const char * str = runningIconPath.c_str();
	str = defaultIconPath.c_str();
	m_defaultIcon = Gdk::Pixbuf::create_from_file(defaultIconPath);
	m_runningIcon = Gdk::Pixbuf::create_from_file(runningIconPath);
	m_statusIcon = Gtk::StatusIcon::create(m_defaultIcon);
	setIcon();

	Gtk::Window::set_default_icon(m_defaultIcon);

	m_timekeeper->attach(this);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Toggle main window"), sigc::mem_fun(*this,
				&StatusIcon::on_menu_file_popup_open)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all timers"), sigc::mem_fun(*this,
				&StatusIcon::on_menu_stop_all_timers)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Quit"), sigc::mem_fun(*this,
				&StatusIcon::on_menu_file_popup_quit)));
	}
	setTooltip();
	m_statusIcon->signal_activate().connect(sigc::mem_fun(this, &StatusIcon::on_activate));
	m_statusIcon->signal_popup_menu().connect(sigc::mem_fun(this, &StatusIcon::on_popup_menu));

	m_taskaccessor->attach(this);
}

StatusIcon::~StatusIcon()
{
	m_timekeeper->detach(this);
	m_taskaccessor->detach(this);
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

void StatusIcon::on_taskUpdated(const Task&)
{
	setTooltip();
};


void StatusIcon::on_runningChanged()
{
	setIcon();
	setTooltip();
}
void StatusIcon::setTooltip()
{
	std::stringstream message;
	if (m_timekeeper->hasRunningTasks())
	{
		std::vector<Task> tasks = m_taskaccessor->getRunningTasks();
		//Figure out start and end of today
		time_t startTime = Utils::getBeginingOfDay(time(0));
		time_t stopTime = Utils::getEndOfDay(time(0));
		std::map<int64_t, TaskTime> times = m_timeaccessor->getTimeList(startTime, stopTime);
		for (int i = 0; i < (int) tasks.size(); i++)
		{
			Task task = tasks.at(i);
			if (i > 0)
			{
				message << endl;
			}
			message << setw(15) << setiosflags(ios::left) <<task.getName();
			std::map<int64_t, TaskTime>::iterator time = times.find(task.getID());
			if (time != times.end())
			{
				TaskTime taskTime = time->second;
				message << " " << Utils::seconds2hhmm(taskTime.duration);
			}

		}
	}
	else
	{
		message << _("TimeIt. the unobtrusive time tracker");
	}
	m_statusIcon->set_tooltip(message.str());
}

void StatusIcon::setIcon()
{
	if (m_timekeeper->hasRunningTasks())
	{
		m_statusIcon->set(m_runningIcon);
		Gtk::Window::set_default_icon(m_runningIcon);
	}
	else
	{
		m_statusIcon->set(m_defaultIcon);
		Gtk::Window::set_default_icon(m_defaultIcon);
	}

}
}
}