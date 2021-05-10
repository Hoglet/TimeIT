#include "details_dialog.h"
#include "libtimeit/utils.h"
#include <ctime>
#include <libtimeit/logic/time_keeper.h>

using namespace libtimeit;
using namespace std;

namespace gui
{

static const int COMPLETE_DAY = 86403;

std::shared_ptr<DetailsDialog> DetailsDialog::create(Database& database, Time_keeper &timeKeeper, Notifier& notifier,GUIFactory  &gui_factory)
{
	shared_ptr<DetailsDialog> return_value(new DetailsDialog(database, timeKeeper, notifier, gui_factory));
	return return_value;
}

DetailsDialog::DetailsDialog(
		Database    &database,
		Time_keeper &timeKeeper,
		Notifier    &notifier,
		GUIFactory  &gui_factory)
		:
		Event_observer(notifier),
		table1(14, 1), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		taskName(""),
		detailList(database, notifier, gui_factory),
		taskTotalTime(""),
		table2(7, 4), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		m_timeAccessor(database),
		m_taskAccessor(database),
		settings_accessor( database ),
		m_timeKeeper(timeKeeper)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	// consider not loading and having these icons in memory multiple times accross multiple classes
	runningIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "running.svg"), 24, 24, true);
	runningIdleIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "running-idle.svg"), 24, 24, true);
	blankIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::image_path(), "blank.svg"), 24, 24, true);

	auto width = (int)settings_accessor.get_int("details_dialog_width", 550);
	auto height = (int)settings_accessor.get_int("details_dialog_height", 700);
	set_default_size( width, height);


	scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	scrolledWindow.set_size_request(400, 600);

	taskName.set_alignment(0.0, 0.5);
	runningImage.set_alignment(0.0, 0.5);
	runningImage.set(blankIcon);
	taskName.set_padding(5, 3);
	taskTotalTime.set_padding(5, 3);
	taskTotalTime.set_alignment(0.0, 0.5);
	table1.attach(taskName, 0, 1, 0, 1);
	table1.attach(runningImage, 1, 2, 0, 1);
	table1.attach(taskTotalTime, 2, 14, 0, 1);
	//Layout

	scrolledWindow.add(detailList);
	get_vbox()->pack_start(table1, Gtk::PACK_SHRINK, 3);
	get_vbox()->pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET, 3);
	show_all_children();

}

DetailsDialog::~DetailsDialog()
{
	settings_accessor.set_int("details_dialog_width", this->get_width());
	settings_accessor.set_int("details_dialog_height", this->get_height());
}


void DetailsDialog::on_selection_changed(int64_t taskID, time_t startTime, time_t stopTime)
{
	set(taskID, startTime, stopTime);
}

void DetailsDialog::on_task_time_changed(int64_t task_ID)
{
	on_task_total_time_updated(task_ID);
}

void DetailsDialog::on_task_name_changed(int64_t task_ID)
{
	on_task_name_updated(task_ID);
}


void DetailsDialog::set(int64_t taskID, time_t startTime, time_t stopTime)
{
	time_entry_id = 0;
	task_id = taskID;
	range_start = startTime;
	range_stop = stopTime;

	on_task_name_updated(taskID);
	on_task_total_time_updated(taskID);

	detailList.set(taskID, range_start, range_stop);
	on_runningTasksChanged();
}



// also displays whether idle
void DetailsDialog::on_runningTasksChanged()
{
	auto running_tasks = m_timeAccessor.currently_running();
	bool running_this_task_id = false;
	if (running_tasks.empty())
	{
		set_title("TimeIT");
	}
	else
	{
		set_title("TimeIT ⌚");
		for (auto task_id_ : running_tasks)
		{
			if (task_id == task_id_)
			{
				running_this_task_id = true;
				break;
			}
		}
	}
	if (running_this_task_id)
	{
		if (!m_timeKeeper.tasks_are_running())
		{
			runningImage.set(runningIcon);
		}
		else
		{
			runningImage.set(runningIdleIcon);
		}
	}
	else
	{
		runningImage.set(blankIcon);
	}
}

void DetailsDialog::on_task_name_updated(int64_t id)
{
	if (task_id == id)
	{
		auto task = m_taskAccessor.by_ID(task_id);
		if (task.has_value())
		{
			taskName.set_text(task->name);
		}
		else
		{
			taskName.set_text("");
		}
	}
	else
	{
		// ignore any other task being updated
	}
}


void DetailsDialog::on_task_total_time_updated(int64_t id)
{
	if (task_id == id)
	{
		if (difftime(range_stop, range_start) > COMPLETE_DAY)
		{
			// longer than a day, could be a week, month, year, with a margin to stay clear of leap seconds
			auto task = m_taskAccessor.by_ID(task_id);
			if (task.has_value())
			{
				time_t total_time = m_timeAccessor.total_cumulative_time(task_id, range_start, range_stop);
				taskTotalTime.set_text("∑ = " + libtimeit::seconds_2_hhmm(total_time));
			}
			else
			{
				taskTotalTime.set_text("");
			}
		}
		else
		{
			// if one day only then don't show here because it shows a daily total in the last row anyway
			taskTotalTime.set_text("");
		}
	}
	else
	{
		// ignore any other task being updated
	}
}

}
