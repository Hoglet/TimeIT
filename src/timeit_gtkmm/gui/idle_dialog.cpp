/*
 * IdleDialog.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include "idle_dialog.h"
#include <iostream>
#include <string>
#include <glibmm/i18n.h>
#include "libtimeit/timer.h"
#include <libtimeit/utils.h>

namespace gui
{

using namespace::std;
using namespace libtimeit;

Idle_dialog::Idle_dialog(
		Timer& timer,
		Database& database,
		Time_keeper& time_keeper_)
		:
		Timer_observer(timer),
		task_accessor(database ),
		time_accessor( database ),
		time_keeper(time_keeper_)
{

	//Setting start time to now in case nobody will set the idle time later.

	set_deletable(false);
	set_text();
	get_vbox()->pack_start(text);
	// This is one answer to the question "No activity have
	// been detected for X minutes. What should we do?"
	add_button(_("Revert and stop"), RESPONSE_REVERT);
	// This is one answer to the question "No activity have
	// been detected for X minutes. What should we do?"
	add_button(_("Revert and continue"), RESPONSE_REVERTANDCONTINUE);
	// This is one answer to the question "No activity has
	// been detected for X minutes. What should we do?"
	add_button(_("Continue"), RESPONSE_CONTINUE);


	signal_response().connect(sigc::mem_fun(this, &Idle_dialog::response_handler));

	//set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);
	show_all_children();
	set_keep_above(true);
}

void Idle_dialog::set_time_id(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if ( time_entry.has_value())
	{
		idle_start_time = time_entry->stop;
		auto task = task_accessor.by_ID(time_entry->task_id);
		if(task.has_value())
		{
			task_string = task->name;
			time_entry_id = id;
		}
	}
}

void Idle_dialog::on_signal_10_seconds()
{
	set_text();
}

void Idle_dialog::show()
{
	set_text();
	Gtk::Dialog::show();
}

static const int SECONDS_PER_MINUTE = 60;

void Idle_dialog::set_text()
{
	std::stringstream str;
	auto minutes_idle = (libtimeit::now() - idle_start_time) / SECONDS_PER_MINUTE;

	// %d represents the time
	std::string format_str = ngettext("No activity has been detected for %d minute. What should we do?",
									  "No activity has been detected for %d minutes. What should we do?",
									  (unsigned long)minutes_idle);
	str << libtimeit::string_printf(format_str, minutes_idle);

	if ( !task_string.empty() )
	{
		str << "\n\n";
		//Context: Before this string will be "No activity has been detected for %d minutes. What should we do?" and after this txt it will be the name of the task
		str << _("Task affected: ");
		str << task_string << std::endl;
	}

	text.set_text(str.str());
}

void Idle_dialog::response_handler(int result)
{
	switch (result)
	{
		case (RESPONSE_REVERT):
			revert_and_stop(time_entry_id);
			break;
		case (RESPONSE_REVERTANDCONTINUE):
			revert_and_continue(time_entry_id);
			break;
		case (RESPONSE_CONTINUE):
			action_continue(time_entry_id);
			break;
		default:
			cout << "Unexpected button clicked." << std::endl;
			break;
	}
	hide();
}

void Idle_dialog::action_continue(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_accessor.update(time_entry->with(RUNNING).with_stop(libtimeit::now()));
	}
}

void Idle_dialog::revert_and_stop(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_keeper.stop_time(id);
		time_keeper.stop(time_entry->task_id);
	}
}

void Idle_dialog::revert_and_continue(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_keeper.stop_time(id);
		time_keeper.start(time_entry->task_id);
	}
}

}
