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
#include <gui/images.h>

namespace gui
{
using namespace std;
using namespace libtimeit;

status_icon_widget::status_icon_widget(
		Time_keeper& time_keeper,
		database&    db,
		notification_manager&    notifier,
		image_cache&      images)
		:
		time_keeper_observer(time_keeper),
		event_observer(notifier),
		m_time_keeper(time_keeper),
		tasks(db),
		times(db)

{
	default_icon = images.by_id(image_identifier::STD_ICON);
	running_icon = images.by_id(image_identifier::RUNNING_BIG);
	status_icon_instance  = Gtk::StatusIcon::create(default_icon);
	set_icon();

	running_icon_small = images.by_id(image_identifier::RUNNING_SMALL);
	idle_icon_small    = images.by_id(image_identifier::BLANK_SMALL);

	Gtk::Window::set_default_icon(default_icon);

	populate_context_menu();
	set_tooltip();
	status_icon_instance->signal_activate().connect(sigc::mem_fun(this, &status_icon_widget::on_activate));
	status_icon_instance->signal_popup_menu().connect(sigc::mem_fun(this, &status_icon_widget::on_popup_menu));
}

void status_icon_widget::populate_context_menu()
{
	Gtk::Menu::MenuList &menu_list = menu_popup.items();
	menu_list.clear();

	latest_tasks = times.latest_active_tasks(5);
	auto running_tasks = times.currently_running();
	for (int i = 0; i < (int) latest_tasks.size(); i++)
	{
		try
		{
			auto id = latest_tasks[i];
			auto task = tasks.by_id(id);
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
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this,
																												  &status_icon_widget::on_menu_toggle_task_1)));
				break;
			case 1:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this,
																												  &status_icon_widget::on_menu_toggle_task_2)));
				break;
			case 2:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this,
																												  &status_icon_widget::on_menu_toggle_task_3)));
				break;
			case 3:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this,
																												  &status_icon_widget::on_menu_toggle_task_4)));
				break;
			case 4:
				menu_list.push_back(Gtk::Menu_Helpers::ImageMenuElem(menu_line.c_str(), *menu_icon, sigc::mem_fun(*this,
																												  &status_icon_widget::on_menu_toggle_task_5)));
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
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Toggle main window"), sigc::mem_fun(*this, &status_icon_widget::on_menu_file_popup_open)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop all timers"), sigc::mem_fun(*this, &status_icon_widget::on_menu_stop_all_timers)));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(
			Gtk::StockID("gtk-quit"),
			[this]()
			{
				this->on_menu_file_popup_quit();
			}
			));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-preferences"), sigc::mem_fun(this, &status_icon_widget::on_menu_preferences)));
	menu_list.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-about"), sigc::mem_fun(this, &status_icon_widget::on_menu_about)));

}

std::string status_icon_widget::complete_task_path(const task_id& id)
{
	std::string task_name;
	auto task = tasks.by_id(id);
	if (task.has_value())
	{
		task_name = task->name;
		if (task->parent_id.has_value() )
		{
			task_name = complete_task_path(task->parent_id.value()) + " / " + task_name;
		}
	}
	return task_name;
}

void status_icon_widget::on_menu_toggle_task_1()
{
	toggle_task(latest_tasks[0]);
}

void status_icon_widget::on_menu_toggle_task_2()
{
	toggle_task(latest_tasks[1]);
}

void status_icon_widget::on_menu_toggle_task_3()
{
	toggle_task(latest_tasks[2]);
}

void status_icon_widget::on_menu_toggle_task_4()
{
	toggle_task(latest_tasks[3]);
}

void status_icon_widget::on_menu_toggle_task_5()
{
	toggle_task(latest_tasks[4]);
}

void status_icon_widget::toggle_task(const task_id& id)
{
	m_time_keeper.toggle(id);
}

void status_icon_widget::on_activate()
{
	toggle_main_window();
}
void status_icon_widget::toggle_main_window()
{
	for (auto* observer: observers)
	{
		observer->on_action_toggle_main_window();
	}
}

void status_icon_widget::attach(action_observer *observer)
{
	observers.push_back(observer);
}
void status_icon_widget::detach(action_observer *observer)
{
	observers.remove(observer);
}

void status_icon_widget::on_menu_file_popup_open()
{
	toggle_main_window();
}
void status_icon_widget::on_menu_file_popup_quit()
{
	toggle_main_window(); //Uggly, but it saves the main window size.
	Gtk::Main::quit();
}
void status_icon_widget::on_menu_stop_all_timers()
{

	for (auto* observer: observers)
	{
		observer->on_action_stop_timers();
	}
}

void status_icon_widget::on_menu_about()
{
	for (auto* observer: observers)
	{
		observer->on_action_about();
	}
}

void status_icon_widget::on_menu_preferences()
{
	for (auto* observer: observers)
	{
		observer->on_action_preferences();
	}
}

void status_icon_widget::on_popup_menu(guint button, guint32 activate_time)
{
	menu_popup.popup(button, activate_time);
}

void status_icon_widget::on_task_updated(const task_id& /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void status_icon_widget::on_task_name_changed(const task& /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void status_icon_widget::on_task_time_changed(const task_id& /*id*/)
{
	set_tooltip();
	populate_context_menu();
}

void status_icon_widget::on_complete_update()
{
	set_tooltip();
	populate_context_menu();
}

void status_icon_widget::on_running_changed()
{
	set_icon();
	set_tooltip();
	populate_context_menu();
}
void status_icon_widget::set_tooltip()
{
	std::stringstream message { };
	auto currently_running = times.currently_running();
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
		for (auto id : currently_running)
		{
			auto task = tasks.by_id(id);
			message << setw(15) << setiosflags(ios::left) << task->name;
			message << " " << libtimeit::seconds_2_hhmm(times.duration_time(id, start_time, stop_time));
		}
	}
	status_icon_instance->set_tooltip(message.str());
}

void status_icon_widget::set_icon()
{
	if (m_time_keeper.has_running_tasks())
	{
		status_icon_instance->set(running_icon);
		Gtk::Window::set_default_icon(running_icon);
	}
	else
	{
		status_icon_instance->set(default_icon);
		Gtk::Window::set_default_icon(default_icon);
	}

}
}
