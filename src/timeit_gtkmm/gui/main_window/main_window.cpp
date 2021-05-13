#include "main_window.h"
#include "task_list.h"
#include <gtkmm.h>
#include <iostream>
#include <libtimeit/utils.h>
#include <libtimeit/OS_abstraction.h>
#include <vector>
#include <glibmm/i18n.h>
#include "summary.h"
#include <libtimeit/db/default_values.h>
#include <glibmm.h>

using namespace Gtk;
using namespace libtimeit;

namespace gui
{

MainWindow::~MainWindow()
{
	empty_containers();
	save_size();
	detach(this);
}

static const int DEFAULT_WINDOW_WIDTH = 550;
static const int DEFAULT_WINDOW_HEIGHT = 550;

MainWindow::MainWindow(
		Database &database,
		Time_keeper &timeKeeper,
		Notifier& notifier,
		Window_manager& gui_factory)
		:
		task_list(database, timeKeeper, notifier),
		day_summary(database, notifier),
		week_summary(database, notifier),
		month_summary(database, notifier),
		year_summary(database, notifier),
		label_day(
				_("Day")),
		label_week(_("Week")),
		label_month(_("Month")),
		label_year(_("Year")),
		details(database, notifier, gui_factory),
		time_accessor(database),
		task_accessor(database),
		settings_accessor(database)
{
	create_layout();
	relate_widgets();
	attach(this);
	show_all_children();
	on_running_tasks_changed();

	auto width = (int)settings_accessor.get_int("main_window_width", DEFAULT_WINDOW_WIDTH);
	auto height = (int)settings_accessor.get_int("main_window_height", DEFAULT_WINDOW_HEIGHT);
	set_default_size( width, height);
	signal_hide().connect(sigc::mem_fun(this, &MainWindow::save_size));
}

void MainWindow::save_size()
{
	settings_accessor.set_int("main_window_width", get_width());
	settings_accessor.set_int("main_window_height", get_height());
	settings_accessor.set_int("main_win_v_paned_position", v_paned.get_position());
	settings_accessor.set_int("main_win_h_paned_position", h_paned.get_position());
}

Calendar& MainWindow::get_calendar()
{
	return calendar;
}

void MainWindow::attach(Summary_observer *observer)
{
	vector<Summary*>::iterator iter;

	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->attach(observer);
	}
}
void MainWindow::detach(Summary_observer *observer)
{
	vector<Summary*>::iterator iter;

	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->detach(observer);
	}
}

void MainWindow::attach(Action_observer *observer)
{
	detach(observer); //To avoid duplicates
	task_list.attach(observer);
	toolbar.attach(observer);
	menubar.attach(observer);
	auto *s_observer = dynamic_cast<Summary_observer*>(observer);
	if (s_observer != nullptr)
	{
		attach(s_observer);
	}
}

void MainWindow::detach(Action_observer *observer)
{
	task_list.detach(observer);
	toolbar.detach(observer);
	menubar.detach(observer);
	auto *s_observer = dynamic_cast<Summary_observer*>(observer);
	if (s_observer != nullptr)
	{
		detach(s_observer);
	}
}

void MainWindow::relate_widgets()
{
	//Give widgets references to other widgets they need too communicate with
	summaries.push_back(&day_summary);
	summaries.push_back(&week_summary);
	summaries.push_back(&month_summary);
	summaries.push_back(&year_summary);

	vector<Summary*>::iterator iter;
	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->set_references(calendar);
		summary->attach(&details);
	}
}

void MainWindow::create_layout()
{
	//First setting parameters on widgets

	task_list_container.set_size_request(270, 230);
	task_list_container.set_flags(Gtk::CAN_FOCUS);
	task_list_container.set_shadow_type(Gtk::SHADOW_NONE);
	task_list_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	calendar.set_flags(Gtk::CAN_FOCUS);
	calendar.display_options(
			Gtk::CALENDAR_SHOW_HEADING | Gtk::CALENDAR_SHOW_DAY_NAMES | Gtk::CALENDAR_SHOW_WEEK_NUMBERS);
	day_summary_container.set_flags(Gtk::CAN_FOCUS);
	day_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	day_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	week_summary_container.set_flags(Gtk::CAN_FOCUS);
	week_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	week_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	month_summary_container.set_flags(Gtk::CAN_FOCUS);
	month_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	month_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	year_summary_container.set_flags(Gtk::CAN_FOCUS);
	year_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	year_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	h_paned.set_flags(Gtk::CAN_FOCUS);
	summary_tabs.set_flags(Gtk::CAN_FOCUS);
	summary_tabs.set_size_request(250, 150);

	//Then the actual layout
	details.set_size_request(100, 100);
	do_layout();
	details_window.add(details);
	details_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);


	h_paned.set_position(settings_accessor.get_int("main_win_h_paned_position", get_width() * 0.4));;
	v_paned.set_position(settings_accessor.get_int("main_win_v_paned_position", 10));
	v_paned.pack2(details_window);

	show_all_children();
	grab_focus();
}

using namespace Glib::Container_Helpers;
void MainWindow::remove_children(Container &container)
{
	Glib::ListHandle<Gtk::Widget*> listh = container.get_children();
	for (auto iter = listh.begin(); iter != listh.end(); ++iter)
	{
		Gtk::Widget *widget = (*iter);
		container.remove(*widget);
	}
}

void MainWindow::empty_containers()
{
	remove_children(*this);
	remove_children(h_paned);
	remove_children(main_v_box);
	remove_children(task_list_container);
	remove_children(secondary_v_box);
	remove_children(day_summary_container);
	remove_children(week_summary_container);
	remove_children(month_summary_container);
	remove_children(year_summary_container);
	remove_children(summary_tabs);
}

void MainWindow::on_settings_changed(string name)
{
	if (name == "CompactLayout")
	{
		do_layout();
	}
}
void MainWindow::do_layout()
{
	if (settings_accessor.get_bool("CompactLayout", DEFAULT_COMPACT_LAYOUT))
	{
		empty_containers();
		default_layout();
	}
	else
	{
		empty_containers();
		classic_layout();
	}
}

void MainWindow::default_layout()
{
	add(v_paned);
	v_paned.pack1(h_paned, Gtk::SHRINK);
	{
		h_paned.pack1(main_v_box, Gtk::SHRINK);
		{
			main_v_box.pack_start(menubar, Gtk::PACK_SHRINK, 0);
			main_v_box.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
			main_v_box.pack_start(task_list_container); // Gtk::AttachOptions(0));

			{
				task_list_container.add(task_list);
			}
		}
		h_paned.pack2(secondary_v_box,  Gtk::EXPAND);
		{
			secondary_v_box.pack_start(calendar, Gtk::PACK_SHRINK, 0);
			secondary_v_box.pack_start(summary_tabs);
			{
				day_summary_container.add(day_summary);
				week_summary_container.add(week_summary);
				month_summary_container.add(month_summary);
				year_summary_container.add(year_summary);
				summary_tabs.append_page(day_summary_container, label_day);
				summary_tabs.append_page(week_summary_container, label_week);
				summary_tabs.append_page(month_summary_container, label_month);
				summary_tabs.append_page(year_summary_container, label_year);

			}
		}

	}

	//mainVBox.pack_start(statusbar, Gtk::PACK_SHRINK, 0);

}

void MainWindow::classic_layout()
{
	add(v_paned);
	v_paned.pack1(main_v_box, Gtk::SHRINK);
	{
		main_v_box.pack_start(menubar, Gtk::PACK_SHRINK, 0);
		main_v_box.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
		main_v_box.pack_start(h_paned);
		{
			h_paned.pack1(task_list_container, Gtk::SHRINK); // Gtk::AttachOptions(0));

			{
				task_list_container.add(task_list);
			}
			h_paned.pack2(secondary_v_box, Gtk::EXPAND);
			{
				secondary_v_box.pack_start(calendar, Gtk::PACK_SHRINK, 0);
				secondary_v_box.pack_start(summary_tabs);
				{
					day_summary_container.add(day_summary);
					week_summary_container.add(week_summary);
					month_summary_container.add(month_summary);
					year_summary_container.add(year_summary);
					summary_tabs.append_page(day_summary_container, label_day);
					summary_tabs.append_page(week_summary_container, label_week);
					summary_tabs.append_page(month_summary_container, label_month);
					summary_tabs.append_page(year_summary_container, label_year);
				}
			}

		}
		main_v_box.pack_start(statusbar, Gtk::PACK_SHRINK, 0);
	}
}

// also displays whether idle
void MainWindow::on_running_tasks_changed()
{
	auto running_tasks = time_accessor.currently_running();
	if (running_tasks.empty() )
	{
		set_title("TimeIT");
	}
	else
	{
		set_title("TimeIT ⌚");
	}
	task_list.on_complete_update();
}

void MainWindow::on_show()
{
	Gtk::Window::on_show();
	Gtk::Window::deiconify();
	set_calendar();
}

static const int YEAR_ZERO = 1900;

void MainWindow::set_calendar()
{
	time_t now = libtimeit::now();
	struct tm *time_info = localtime(&now);
	auto month = (guint)time_info->tm_mon;
	auto year  = (guint)time_info->tm_year + YEAR_ZERO;
	auto day   = (guint)time_info->tm_mday;
	calendar.select_month(month, year);
	calendar.select_day(day);
}

void MainWindow::on_action_task_selection_changed(Task_id selected_task_id)
{
	toolbar.set_task_is_selected(selected_task_id > 0);
	menubar.set_task_is_selected(selected_task_id > 0);
}

void MainWindow::on_action_remove_task()
{
	auto selected_task_id = task_list.selected_id();
	//Confirm dialog when removing task, headline!
	const char *headline = _("Are you sure you want to delete this task?");
	Gtk::MessageDialog dialog(*this, headline, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL);
	//Line after "Are you sure you want to delete this task?" inside the dialog
	dialog.set_secondary_text(_("There is no undelete function"));
	dialog.set_deletable(false);

	int result = dialog.run();

	//Handle the response:
	switch (result)
	{
	case (Gtk::RESPONSE_OK):
		//Remove task
		task_accessor.remove(selected_task_id);
		break;
	case (Gtk::RESPONSE_CANCEL):
		break;
	default:
		cerr << "Unexpected button clicked." << std::endl;
		break;
	}
}

}
