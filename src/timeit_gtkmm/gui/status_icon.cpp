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

namespace gui
{
using namespace std;
using namespace libtimeit;

Status_icon::Status_icon(
		Time_keeper &time_keeper,
		Database &database,
		Notifier& notifier)
		:
		Time_keeper_observer(time_keeper),
		Event_observer(notifier),
		m_time_keeper(time_keeper),
		task_accessor(database),
		time_accessor(database)

{
	auto image_path = libtimeit::image_path();
	auto default_icon_path = Glib::build_filename(image_path, "icon.svg");
	auto running_icon_path = Glib::build_filename(image_path, "running.svg");
	auto blank_icon_path = Glib::build_filename(image_path, "blank.png");

	default_icon = Gdk::Pixbuf::create_from_file(default_icon_path);
	running_icon = Gdk::Pixbuf::create_from_file(running_icon_path);
	status_icon = Gtk::StatusIcon::create(default_icon);
	set_icon();

	running_icon_small = Gdk::Pixbuf::create_from_file(running_icon_path, 15, 15, true);
	idle_icon_small = Gdk::Pixbuf::create_from_file(blank_icon_path, 15, 15, true);

	Gtk::Window::set_default_icon(default_icon);

	populate_context_menu();
	set_tooltip();
	status_icon->signal_activate().connect(sigc::mem_fun(this, &Status_icon::on_activate));
	status_icon->signal_popup_menu().connect(sigc::mem_fun(this, &Status_icon::on_popup_menu));
}

void Status_icon::populate_context_menu()
{
	Gtk::Menu::MenuList &menu_list = menu_popup.items();
	menu_list.clear();

	latest_tasks = time_accessor.latest_active_tasks(5);
	std::vector<int64_t> running_tasks = time_accessor.currently_running();
	for (int i = 0; i < (int) latest_tasks.size(); i++)
	{
		try
		{
			int64_t id = latest_tasks[i];
			auto task = task_accessor.by_ID(id);
			string menu_line = complete_task_path(latest_tasks[i]);

			Gtk::Image *menu_icon = Gtk::manage(new Gtk::Image());
			if (find(running_tasks.begin(), running_tasks.end(), id) != running_tasks.end())
			{
				menu_icon->set(running_icon_small);
			}
			else
			{
				menu_icon->set(idle_icon_small);
			}

			switch (i)
			{
			case 0:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this, &Status_icon::on_menu_toggle_task1)));
				break;
			case 1:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this, &Status_icon::on_menu_toggle_task2)));
				break;
			case 2:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this, &Status_icon::on_menu_toggle_task3)));
				break;
			case 3:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this, &Status_icon::on_menu_toggle_task4)));
				break;
			case 4:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this, &Status_icon::on_menu_toggle_task5)));
				break;
			default:
				break;
			}
		}
		catch (...)
		{
			//Ignore all errors
		}
	}
	menu_list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Toggle main window"), sigc::mem_fun(*this, &Status_icon::on_menu_file_popup_open)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all timers"), sigc::mem_fun(*this, &Status_icon::on_menu_stop_all_timers)));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(
			Gtk::StockID("gtk-quit"),
			[]()
			{
				Status_icon::on_menu_file_popup_quit();
			}
			));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &Status_icon::on_menu_preferences)));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &Status_icon::on_menu_about)));

}

std::string Status_icon::complete_task_path(int64_t id)
{
	std::string task_name;
	auto task = task_accessor.by_ID(id);
	if (task.has_value())
	{
		task_name = task->name;
		if (task->parent_id > 0)
		{
			task_name = complete_task_path(task->parent_id) + " / " + task_name;
		}
	}
	return task_name;
}

void Status_icon::on_menu_toggle_task1()
{
	toggle_task(latest_tasks[0]);
}

void Status_icon::on_menu_toggle_task2()
{
	toggle_task(latest_tasks[1]);
}

void Status_icon::on_menu_toggle_task3()
{
	toggle_task(latest_tasks[2]);
}

void Status_icon::on_menu_toggle_task4()
{
	toggle_task(latest_tasks[3]);
}

void Status_icon::on_menu_toggle_task5()
{
	toggle_task(latest_tasks[4]);
}

void Status_icon::toggle_task(int64_t id)
{
	m_time_keeper.toggle(id);
}

void Status_icon::on_activate()
{
	toggle_main_window();
}
void Status_icon::toggle_main_window()
{
	for (auto* observer: observers)
	{
		observer->on_action_toggle_main_window();
	}
}

void Status_icon::attach(Action_observer *observer)
{
	observers.push_back(observer);
}
void Status_icon::detach(Action_observer *observer)
{
	observers.remove(observer);
}

void Status_icon::on_menu_file_popup_open()
{
	toggle_main_window();
}
void Status_icon::on_menu_file_popup_quit()
{
	Gtk::Main::quit();
}
void Status_icon::on_menu_stop_all_timers()
{

	for (auto* observer: observers)
	{
		observer->on_action_stop_timers();
	}
}

void Status_icon::on_menu_about()
{
	for (auto* observer: observers)
	{
		observer->on_action_about();
	}
}

void Status_icon::on_menu_preferences()
{
	for (auto* observer: observers)
	{
		observer->on_action_preferences();
	}
}

void Status_icon::on_popup_menu(guint button, guint32 activate_time)
{
	menu_popup.popup(button, activate_time);
}

void Status_icon::on_task_updated(Task_id /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void Status_icon::on_task_name_changed(Task_id /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void Status_icon::on_task_time_changed(Task_id /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void Status_icon::on_complete_update()
{
	set_tooltip();
	populate_context_menu();
}

void Status_icon::on_running_changed()
{
	set_icon();
	set_tooltip();
	populate_context_menu();
}
void Status_icon::set_tooltip()
{
	std::stringstream message { };
	auto currently_running = time_accessor.currently_running();
	if (currently_running.empty() )
	{
		//Tagline
		message << _("TimeIT. the unobtrusive time tracker");
	}
	else
	{
		//Figure out start and end of today
		auto start_time = libtimeit::beginning_of_day(time(nullptr));
		auto stop_time  = libtimeit::end_of_day(time(nullptr));
		for (int64_t id : currently_running)
		{
			auto task = task_accessor.by_ID(id);
			message << setw(15) << setiosflags(ios::left) << task->name;
			message << " " << libtimeit::seconds_2_hhmm(time_accessor.duration_time(id, start_time, stop_time));
		}
	}
	status_icon->set_tooltip(message.str());
}

void Status_icon::set_icon()
{
	if (m_time_keeper.hasRunningTasks())
	{
		status_icon->set(running_icon);
		Gtk::Window::set_default_icon(running_icon);
	}
	else
	{
		status_icon->set(default_icon);
		Gtk::Window::set_default_icon(default_icon);
	}

}
}
