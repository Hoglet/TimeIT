/*
 * IdleDialog.cpp
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#include "idle_dialog.h"
#include <iostream>
#include <string>
#include <sstream>
#include <glibmm/i18n.h>
#include "libtimeit/timer.h"
#include <libtimeit/utils.h>

namespace GUI
{

using namespace::std;
using namespace libtimeit;

IdleDialog::IdleDialog(
		Timer& timer,
		Database& database,
		Time_keeper& time_keeper_)
		:
		Timer_observer(timer),
		taskAccessor(database ),
		time_accessor( database ),
		time_keeper(time_keeper_)
{

	//Setting start time to now in case nobody will set the idle time later.

	set_deletable(false);
	setText();
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


	signal_response().connect(sigc::mem_fun(this, &IdleDialog::responseHandler));

	//set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);
	show_all_children();
	set_keep_above(true);
}

void IdleDialog::set_time_id(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if ( time_entry.has_value())
	{
		m_idleStartTime = time_entry->stop;
		auto task = taskAccessor.by_ID(time_entry->task_id);
		if(task.has_value())
		{
			taskString = task->name;
			time_entry_id = id;
		}
	}
}

void IdleDialog::on_signal_10_seconds()
{
	setText();
}

void IdleDialog::show()
{
	setText();
	Gtk::Dialog::show();
}

void IdleDialog::setText()
{
	std::stringstream str;
	int minutesIdle = (libtimeit::now() - m_idleStartTime) / 60;

	// %d represents the time
	std::string format_str = ngettext("No activity has been detected for %d minute. What should we do?",
			"No activity has been detected for %d minutes. What should we do?",
			minutesIdle);
	str << libtimeit::string_printf(format_str, minutesIdle);

	if (taskString.size() > 0)
	{
		str << "\n\n";
		//Context: Before this string will be "No activity has been detected for %d minutes. What should we do?" and after this txt it will be the name of the task
		str << _("Task affected: ");
		str << taskString << std::endl;
	}

	text.set_text(str.str());
}

void IdleDialog::responseHandler(int result)
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

void IdleDialog::action_continue(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_accessor.update(time_entry->with(RUNNING).with_stop(libtimeit::now()));
	}
}

void IdleDialog::revert_and_stop(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_keeper.stop(time_entry->task_id);
	}
}

void IdleDialog::revert_and_continue(Time_id id)
{
	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_keeper.stop_time(id);
		time_keeper.start(time_entry->task_id);
	}
}

}
