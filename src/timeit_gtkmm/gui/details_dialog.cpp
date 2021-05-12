#include "details_dialog.h"
#include "libtimeit/utils.h"
#include <ctime>
#include <libtimeit/logic/time_keeper.h>

using namespace libtimeit;
using namespace std;

namespace gui
{

static const int COMPLETE_DAY = 86403;

std::shared_ptr<Details_dialog> Details_dialog::create(Database& database, Time_keeper &timeKeeper, Notifier& notifier, Window_manager  &gui_factory)
{
	shared_ptr<Details_dialog> return_value(new Details_dialog(database, timeKeeper, notifier, gui_factory));
	return return_value;
}

Details_dialog::Details_dialog(
		Database    &database,
		Time_keeper &timeKeeper,
		Notifier    &notifier,
		Window_manager  &gui_factory)
		:
		Event_observer(notifier),
		table1(14, 1), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		task_name(""),
		detail_list(database, notifier, gui_factory),
		task_total_time(""),
		table2(7, 4), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		time_accessor(database),
		task_accessor(database),
		settings_accessor( database ),
		time_keeper(timeKeeper)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	// consider not loading and having these icons in memory multiple times accross multiple classes
	running_icon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "running.svg"), 24, 24, true);
	running_idle_icon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "running-idle.svg"), 24, 24, true);
	blank_icon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "blank.svg"), 24, 24, true);

	auto width = (int)settings_accessor.get_int("details_dialog_width", 550);
	auto height = (int)settings_accessor.get_int("details_dialog_height", 700);
	set_default_size( width, height);


	scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	scrolled_window.set_size_request(400, 600);

	task_name.set_alignment(0.0, 0.5);
	running_image.set_alignment(0.0, 0.5);
	running_image.set(blank_icon);
	task_name.set_padding(5, 3);
	task_total_time.set_padding(5, 3);
	task_total_time.set_alignment(0.0, 0.5);
	table1.attach(task_name, 0, 1, 0, 1);
	table1.attach(running_image, 1, 2, 0, 1);
	table1.attach(task_total_time, 2, 14, 0, 1);
	//Layout

	scrolled_window.add(detail_list);
	get_vbox()->pack_start(table1, Gtk::PACK_SHRINK, 3);
	get_vbox()->pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET, 3);
	show_all_children();

}

Details_dialog::~Details_dialog()
{
	settings_accessor.set_int("details_dialog_width", this->get_width());
	settings_accessor.set_int("details_dialog_height", this->get_height());
}


void Details_dialog::on_selection_changed(int64_t taskID, time_t startTime, time_t stopTime)
{
	set(taskID, startTime, stopTime);
}

void Details_dialog::on_task_time_changed(int64_t task_ID)
{
	on_task_total_time_updated(task_ID);
}

void Details_dialog::on_task_name_changed(int64_t task_ID)
{
	on_task_name_updated(task_ID);
}


void Details_dialog::set(int64_t taskID, time_t startTime, time_t stopTime)
{
	time_entry_id = 0;
	task_id = taskID;
	range_start = startTime;
	range_stop = stopTime;

	on_task_name_updated(taskID);
	on_task_total_time_updated(taskID);

	detail_list.set(taskID, range_start, range_stop);
	on_running_tasks_changed();
}



// also displays whether idle
void Details_dialog::on_running_tasks_changed()
{
	auto running_tasks = time_accessor.currently_running();
	bool running_this_task_id = false;
	if (running_tasks.empty())
	{
		set_title("TimeIT");
	}
	else
	{
		set_title("TimeIT ⌚");
		for (auto id : running_tasks)
		{
			if (task_id == id)
			{
				running_this_task_id = true;
				break;
			}
		}
	}
	if (running_this_task_id)
	{
		if (!time_keeper.tasks_are_running())
		{
			running_image.set(running_icon);
		}
		else
		{
			running_image.set(running_idle_icon);
		}
	}
	else
	{
		running_image.set(blank_icon);
	}
}

void Details_dialog::on_task_name_updated(int64_t id)
{
	if (task_id == id)
	{
		auto task = task_accessor.by_ID(task_id);
		if (task.has_value())
		{
			task_name.set_text(task->name);
		}
		else
		{
			task_name.set_text("");
		}
	}
	else
	{
		// ignore any other task being updated
	}
}


void Details_dialog::on_task_total_time_updated(int64_t id)
{
	if (task_id == id)
	{
		if (difftime(range_stop, range_start) > COMPLETE_DAY)
		{
			// longer than a day, could be a week, month, year, with a margin to stay clear of leap seconds
			auto task = task_accessor.by_ID(task_id);
			if (task.has_value())
			{
				time_t total_time = time_accessor.total_cumulative_time(task_id, range_start, range_stop);
				task_total_time.set_text("∑ = " + libtimeit::seconds_2_hhmm(total_time));
			}
			else
			{
				task_total_time.set_text("");
			}
		}
		else
		{
			// if one day only then don't show here because it shows a daily total in the last row anyway
			task_total_time.set_text("");
		}
	}
	else
	{
		// ignore any other task being updated
	}
}

}
