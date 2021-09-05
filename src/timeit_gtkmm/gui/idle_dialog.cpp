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
#include <fmt/core.h>

namespace gui
{

using namespace::std;
using namespace libtimeit;

idle_dialog::idle_dialog(
		Timer& timer,
		database& db,
		Time_keeper& op_time_keeper)
		:
		timer_observer(timer),
		tasks(db ),
		times(db ),
		time_keeper( op_time_keeper)
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


	signal_response().connect(sigc::mem_fun(this, &idle_dialog::response_handler));

	//set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);
	show_all_children();
	set_keep_above(true);
}

void idle_dialog::set_time_id(const time_id& id)
{
	auto item = times.by_id( id);
	if ( item.has_value() )
	{
		idle_start_time = item->stop;
		auto idle_task = tasks.by_id( item->owner_id);
		if(idle_task.has_value())
		{
			task_string = idle_task->name;
			time_entry_id = id;
		}
	}
}

void idle_dialog::on_signal_10_seconds()
{
	set_text();
}

void idle_dialog::show()
{
	set_text();
	Gtk::Dialog::show();
}

static const int SECONDS_PER_MINUTE = 60;

void idle_dialog::set_text()
{
	std::stringstream str;
	auto minutes_idle = duration_cast<minutes>( system_clock::now() - idle_start_time);

	// {} represents the time
	std::string format_str = ngettext("No activity has been detected for {} minute. What should we do?",
									  "No activity has been detected for {} minutes. What should we do?",
									  minutes_idle.count());
	str << fmt::format(format_str, minutes_idle.count());

	if ( !task_string.empty() )
	{
		str << "\n\n";
		//Context: Before this string will be "No activity has been detected for %d minutes. What should we do?" and after this txt it will be the name of the task
		str << _("Task affected: ");
		str << task_string << std::endl;
	}

	text.set_text(str.str());
}

void idle_dialog::response_handler(int result)
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

void idle_dialog::action_continue(const time_id& id)
{
	auto item = times.by_id( id);
	if(item.has_value())
	{
		times.update( item->with( RUNNING).with_stop( system_clock::now()));
	}
}

void idle_dialog::revert_and_stop(const time_id& id)
{
	auto item = times.by_id( id);
	if(item.has_value())
	{
		time_keeper.stop_time( item.value());
		time_keeper.stop( item->owner_id);
	}
}

void idle_dialog::revert_and_continue(const time_id& id)
{
	auto item = times.by_id( id);
	if(item.has_value())
	{
		time_keeper.stop_time( item.value());
		time_keeper.start( item->owner_id);
	}
}

}
