#include "details_dialog.h"
#include "libtimeit/utils.h"

using namespace libtimeit;
using namespace std;

namespace gui
{

static const int COMPLETE_DAY = 86403;


details_dialog::details_dialog(
		database&             db,
		time_manager&         op_time_keeper,
		notification_manager& notifier,
		window_manager&       gui_factory,
		image_cache&          images)
		:
		event_observer(notifier),
		time_manager_observer( op_time_keeper ),
		top_table( 14, 1), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		task_name(""),
		detail_list(db, notifier, gui_factory),
		task_total_time(""),
		times(db),
		tasks(db),
		settings(db ),
		time_keeper( op_time_keeper)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	running_icon      = images.by_id(image_identifier::RUNNING);
	running_idle_icon = images.by_id(image_identifier::RUNNING_IDLE);
	blank_icon        = images.by_id(image_identifier::BLANK);

	auto width = (int)settings.get_int("details_dialog_width", 550);
	auto height = (int)settings.get_int("details_dialog_height", 700);
	set_default_size( width, height);


	scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	scrolled_window.set_size_request(400, 400);

	task_name.set_alignment(0.0, 0.5);
	running_image.set_alignment(0.0, 0.5);
	running_image.set(blank_icon);
	task_name.set_padding(5, 3);
	task_total_time.set_padding(5, 3);
	task_total_time.set_alignment(0.0, 0.5);
	top_table.attach( task_name, 0, 1, 0, 1);
	top_table.attach( running_image, 1, 2, 0, 1);
	top_table.attach( task_total_time, 2, 14, 0, 1);
	//Layout

	scrolled_window.add(detail_list);
	get_vbox()->pack_start( top_table, Gtk::PACK_SHRINK, 3);
	get_vbox()->pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET, 3);
	show_all_children();

}

details_dialog::~details_dialog()
{
	settings.set_int("details_dialog_width", this->get_width());
	settings.set_int("details_dialog_height", this->get_height());
}


void details_dialog::on_selection_changed( optional<task_id> id, time_point<system_clock> start_time, time_point<system_clock> stop_time)
{
	set( id, start_time, stop_time);
}

void details_dialog::on_task_time_changed(const task_id& id)
{
	on_task_total_time_updated(id);
}

void details_dialog::on_task_name_changed(const task& item)
{
	on_task_name_updated(item.id);
}


void details_dialog::set( optional<task_id> id, time_point<system_clock> start_time, time_point<system_clock> stop_time)
{
	time_entry_id = {};
	presented_task = id;
	range_start = start_time;
	range_stop = stop_time;

	if(id.has_value())
	{
		on_task_name_updated(id.value());
		on_task_total_time_updated(id.value());
	}
	detail_list.set(id, range_start, range_stop);
	on_running_changed();
}



// also displays whether idle
void details_dialog::on_running_changed()
{
	auto running_tasks = times.currently_running();
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
			if (presented_task == id)
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

void details_dialog::on_task_name_updated(const task_id& id)
{
	if (presented_task == id)
	{
		auto updated_task = tasks.by_id(presented_task.value());
		if (updated_task.has_value())
		{
			task_name.set_text(updated_task->name);
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


void details_dialog::on_task_total_time_updated(const task_id& id)
{
	if (presented_task == id)
	{
		if ( range_stop - range_start > 24h )
		{
			// longer than a day, could be a week, month, year, with a margin to stay clear of leap seconds
			auto updated_task = tasks.by_id(presented_task.value());
			if (updated_task.has_value())
			{
				auto total_time = times.total_cumulative_time(presented_task.value(), range_start, range_stop);
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
