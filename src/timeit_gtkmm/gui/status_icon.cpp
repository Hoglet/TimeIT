/*
 * StatusIcon.cpp
 *
 *  Created on: 21-Jun-08
 *      Author: hoglet
 */

#include "status_icon.h"
#include <libtimeit/utils.h>
#include <iostream>
#include <glibmm/i18n.h>
#include <iomanip>

namespace GUI
{
using namespace std;
using namespace libtimeit;

StatusIcon::StatusIcon(
		Time_keeper &time_keeper,
		Database &database,
		Notifier& notifier)
		:
		Event_observer(notifier),
		m_timeKeeper(time_keeper),
		m_taskaccessor(database),
		m_timeaccessor(database)

{
	const std::string &imagePath = libtimeit::image_path();
	std::string defaultIconPath = Glib::build_filename(imagePath, "icon.svg");
	std::string runningIconPath = Glib::build_filename(imagePath, "running.svg");
	std::string blankIconPath = Glib::build_filename(imagePath, "blank.png");

	m_defaultIcon = Gdk::Pixbuf::create_from_file(defaultIconPath);
	m_runningIcon = Gdk::Pixbuf::create_from_file(runningIconPath);
	m_statusIcon = Gtk::StatusIcon::create(m_defaultIcon);
	setIcon();

	runningIconSmall = Gdk::Pixbuf::create_from_file(runningIconPath, 15, 15, true);
	idleIconSmall = Gdk::Pixbuf::create_from_file(blankIconPath, 15, 15, true);

	Gtk::Window::set_default_icon(m_defaultIcon);

	m_timeKeeper.attach(this);

	populateContextMenu();
	setTooltip();
	m_statusIcon->signal_activate().connect(sigc::mem_fun(this, &StatusIcon::on_activate));
	m_statusIcon->signal_popup_menu().connect(sigc::mem_fun(this, &StatusIcon::on_popup_menu));
}

StatusIcon::~StatusIcon()
{
	m_timeKeeper.detach(this);
}

void StatusIcon::populateContextMenu()
{
	Gtk::Menu::MenuList &menulist = m_Menu_Popup.items();
	menulist.clear();

	latestTasks = m_timeaccessor.latest_active_tasks(5);
	std::vector<int64_t> runningTasks = m_timeaccessor.currently_running();
	for (int i = 0; i < (int) latestTasks.size(); i++)
	{
		try
		{
			int64_t id = latestTasks[i];
			auto task = m_taskaccessor.by_ID(id);
			string menuLine = completeTaskPath(latestTasks[i]);

			Gtk::Image *menuIcon = Gtk::manage(new Gtk::Image());
			if (find(runningTasks.begin(), runningTasks.end(), id) != runningTasks.end())
			{
				menuIcon->set(runningIconSmall);
			}
			else
			{
				menuIcon->set(idleIconSmall);
			}

			switch (i)
			{
			case 0:
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(menuLine.c_str(), *menuIcon, sigc::mem_fun(*this, &StatusIcon::on_menu_toggle_task1)));
				break;
			case 1:
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(menuLine.c_str(), *menuIcon, sigc::mem_fun(*this, &StatusIcon::on_menu_toggle_task2)));
				break;
			case 2:
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(menuLine.c_str(), *menuIcon, sigc::mem_fun(*this, &StatusIcon::on_menu_toggle_task3)));
				break;
			case 3:
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(menuLine.c_str(), *menuIcon, sigc::mem_fun(*this, &StatusIcon::on_menu_toggle_task4)));
				break;
			case 4:
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(menuLine.c_str(), *menuIcon, sigc::mem_fun(*this, &StatusIcon::on_menu_toggle_task5)));
				break;
			}
		}
		catch (...)
		{
			//Ignore all errors
		}
	}
	menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Toggle main window"), sigc::mem_fun(*this, &StatusIcon::on_menu_file_popup_open)));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all timers"), sigc::mem_fun(*this, &StatusIcon::on_menu_stop_all_timers)));
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-quit"), sigc::mem_fun(*this, &StatusIcon::on_menu_file_popup_quit)));
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &StatusIcon::on_menu_preferences)));
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &StatusIcon::on_menu_about)));

}

std::string StatusIcon::completeTaskPath(int64_t id)
{
	std::string taskName = "";
	auto task = m_taskaccessor.by_ID(id);
	if (task.has_value())
	{
		taskName = task->name;
		if (task->parent_ID > 0)
		{
			taskName = completeTaskPath(task->parent_ID) + " / " + taskName;
		}
	}
	return taskName;
}

void StatusIcon::on_menu_toggle_task1()
{
	toggleTask(latestTasks[0]);
}

void StatusIcon::on_menu_toggle_task2()
{
	toggleTask(latestTasks[1]);
}

void StatusIcon::on_menu_toggle_task3()
{
	toggleTask(latestTasks[2]);
}

void StatusIcon::on_menu_toggle_task4()
{
	toggleTask(latestTasks[3]);
}

void StatusIcon::on_menu_toggle_task5()
{
	toggleTask(latestTasks[4]);
}

void StatusIcon::toggleTask(int64_t id)
{
	m_timeKeeper.toggle(id);
}

void StatusIcon::on_activate()
{
	toggleMainWindow();
}
void StatusIcon::toggleMainWindow()
{
	std::list<action_observer*>::iterator iter = observers.begin();
	for (; iter != observers.end(); ++iter)
	{
		action_observer *observer = *iter;
		observer->on_action_toggleMainWindow();
	}
}

void StatusIcon::attach(action_observer *observer)
{
	observers.push_back(observer);
}
void StatusIcon::detach(action_observer *observer)
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
	std::list<action_observer*>::iterator iter = observers.begin();
	for (; iter != observers.end(); ++iter)
	{
		action_observer *observer = *iter;
		observer->on_action_stopTimers();
	}
}

void StatusIcon::on_menu_about()
{
	std::list<action_observer*>::iterator iter = observers.begin();
	for (; iter != observers.end(); ++iter)
	{
		action_observer *observer = *iter;
		observer->on_action_about();
	}
}

void StatusIcon::on_menu_preferences()
{
	std::list<action_observer*>::iterator iter = observers.begin();
	for (; iter != observers.end(); ++iter)
	{
		action_observer *observer = *iter;
		observer->on_action_preferences();
	}
}

void StatusIcon::on_popup_menu(guint button, guint32 activate_time)
{
	m_Menu_Popup.popup(button, activate_time);
}

void StatusIcon::on_task_updated(int64_t)
{
	setTooltip();
	populateContextMenu();
}

void StatusIcon::on_task_name_changed(int64_t)
{
	setTooltip();
	populateContextMenu();
}

void StatusIcon::on_task_time_changed(int64_t)
{
	setTooltip();
	populateContextMenu();
}

void StatusIcon::on_complete_update()
{
	setTooltip();
	populateContextMenu();
}

void StatusIcon::on_running_changed()
{
	setIcon();
	setTooltip();
	populateContextMenu();
}
void StatusIcon::setTooltip()
{
	std::stringstream message { };
	std::vector<int64_t> taskIDs = m_timeaccessor.currently_running();
	if (taskIDs.size() > 0)
	{
		//Figure out start and end of today
		time_t startTime = libtimeit::beginning_of_day(time(0));
		time_t stopTime = libtimeit::end_of_day(time(0));
		for (int64_t id : taskIDs)
		{
			auto task = m_taskaccessor.by_ID(id);
			message << setw(15) << setiosflags(ios::left) << task->name;
			message << " " << libtimeit::seconds_2_hhmm(m_timeaccessor.duration_time(id, startTime, stopTime));
		}
	}
	else
	{
		//Tagline
		message << _("TimeIT. the unobtrusive time tracker");
	}
	m_statusIcon->set_tooltip(message.str());
}

void StatusIcon::setIcon()
{
	if (m_timeKeeper.hasRunningTasks())
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
