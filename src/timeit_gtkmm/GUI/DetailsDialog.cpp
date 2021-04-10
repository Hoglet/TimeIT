/*
 * DetailsDialog.cpp
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#include "DetailsDialog.h"
#include "libtimeit/utils.h"
#include <ctime>
#include <glibmm/i18n.h>
#include <libtimeit/logic/time_keeper.h>

using namespace libtimeit;
using namespace std;

namespace GUI
{
std::shared_ptr<DetailsDialog> DetailsDialog::create(Database& database, ITimeKeeper &timeKeeper, Notifier& notifier)
{
	shared_ptr<DetailsDialog> retVal(new DetailsDialog(database, timeKeeper, notifier));
	retVal->weak_this_ptr = weak_ptr<DetailsDialog>(retVal);
	return retVal;
}

DetailsDialog::DetailsDialog(
		Database    &database,
		ITimeKeeper &timeKeeper,
		Notifier    &notifier)
		:
		Event_observer(notifier),
		table1(14, 1),
		taskName(""),
		detailList(database, notifier),
		taskTotalTime(""),
		table2(7, 4),
		startTimeLabel(_("Start time")),
		stopTimeLabel(_("Stop time")),
		startColonLabel(":"),
		toLabel("→"),
		stopColonLabel(":"),
		CancelButton(Gtk::StockID("gtk-revert-to-saved")),
		OKButton(Gtk::StockID("gtk-apply")),
		m_timeAccessor(database),
		m_taskAccessor(database),
		settings_accessor( database ),
		m_timeKeeper(timeKeeper)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	// consider not loading and having these icons in memory multiple times accross multiple classes
	runningIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::getImagePath(), "running.svg"), 24, 24, true);
	runningIdleIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::getImagePath(), "running-idle.svg"), 24, 24, true);
	blankIcon = Gdk::Pixbuf::create_from_file(Glib::build_filename(libtimeit::getImagePath(), "blank.svg"), 24, 24, true);

	startTimeHour.set_range(0, 23);
	startTimeMinute.set_range(0, 59);
	stopTimeHour.set_range(0, 23);
	stopTimeMinute.set_range(0, 59);
	startTimeHour.set_increments(1, 1);
	startTimeMinute.set_increments(1, 1);
	stopTimeHour.set_increments(1, 1);
	stopTimeMinute.set_increments(1, 1);

	auto width = settings_accessor.get_int("details_dialog_width", 550);
	auto height = settings_accessor.get_int("details_dialog_height", 700);
	set_default_size( width, height);


	scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	scrolledWindow.set_size_request(400, 600);

	startTime = 0;
	oldStartTime = 1;
	stopTime = 0;
	oldStopTime = 1;

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
	table2.attach(startTimeLabel, 0, 3, 0, 1);
	table2.attach(stopTimeLabel, 4, 7, 0, 1);
 	table2.attach(startTimeHour, 0, 1, 1, 2);
	table2.attach(startColonLabel, 1, 2, 1, 2);
	table2.attach(startTimeMinute, 2, 3, 1, 2);
	table2.attach(toLabel, 3, 4, 1, 2);
	table2.attach(stopTimeHour, 4, 5, 1, 2);
	table2.attach(stopColonLabel, 5, 6, 1, 2);
	table2.attach(stopTimeMinute, 6, 7, 1, 2);

	scrolledWindow.add(detailList);
	get_vbox()->pack_start(table1, Gtk::PACK_SHRINK, 3);
	get_vbox()->pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(table2, Gtk::PACK_SHRINK, 3);
	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(CancelButton);
	get_action_area()->pack_start(OKButton);
	show_all_children();

	//Connect signals
	OKButton.signal_clicked().connect(sigc::mem_fun(this,
			&DetailsDialog::on_OKButton_clicked));
	CancelButton.signal_clicked().connect(sigc::mem_fun(this,
			&DetailsDialog::on_CancelButton_clicked));
	startTimeHour.signal_value_changed().connect(sigc::mem_fun(this,
			&DetailsDialog::on_change));
	startTimeMinute.signal_value_changed().connect(sigc::mem_fun(this,
			&DetailsDialog::on_change));
	stopTimeHour.signal_value_changed().connect(sigc::mem_fun(this,
			&DetailsDialog::on_change));
	stopTimeMinute.signal_value_changed().connect(sigc::mem_fun(this,
			&DetailsDialog::on_change));

	detailList.attach(this);

	setTimeEntryID(0);
	checkForChanges();
}

DetailsDialog::~DetailsDialog()
{
	settings_accessor.set_int("details_dialog_width", this->get_width());
	settings_accessor.set_int("details_dialog_height", this->get_height());
}

void DetailsDialog::on_OKButton_clicked()
{
	if (startTime != oldStartTime)
	{
		auto te = m_timeAccessor.by_ID(m_timeEntryID);
		if(te)
		{
			m_timeAccessor.update(te->with_start(startTime));
		}
	}
	if (stopTimeHour.sensitive())
	{
		auto te = m_timeAccessor.by_ID(m_timeEntryID);
		if(te)
		{
			m_timeAccessor.update(te->with_stop(stopTime));
		}
	}
	detailList.set(m_taskID, rangeStart, rangeStop);
	oldStartTime = startTime;
	oldStopTime = stopTime;
	checkForChanges();
}
void DetailsDialog::on_CancelButton_clicked()
{
	setValues();
}

void DetailsDialog::on_change()
{
	struct tm startTimeInfo = *localtime(&oldStartTime);
	struct tm stopTimeInfo = *localtime(&oldStopTime);
	startTimeInfo.tm_hour = startTimeHour.get_value_as_int();
	startTimeInfo.tm_min = startTimeMinute.get_value_as_int();
	stopTimeInfo.tm_hour = stopTimeHour.get_value_as_int();
	stopTimeInfo.tm_min = stopTimeMinute.get_value_as_int();
	startTime = mktime(&startTimeInfo);
	stopTime = mktime(&stopTimeInfo);
	checkForChanges();
}

void DetailsDialog::on_selected_changed()
{
	setTimeEntryID(detailList.getSelectedID());
}

void DetailsDialog::setValues()
{
	struct tm startTimeInfo = *localtime(&oldStartTime);
	struct tm stopTimeInfo = *localtime(&oldStopTime);
	startTimeHour.set_value(startTimeInfo.tm_hour);
	startTimeMinute.set_value(startTimeInfo.tm_min);
	stopTimeHour.set_value(stopTimeInfo.tm_hour);
	stopTimeMinute.set_value(stopTimeInfo.tm_min);
}

void DetailsDialog::on_selection_changed(int64_t taskID, time_t startTime, time_t stopTime)
{
	set(taskID, startTime, stopTime);
}

void DetailsDialog::on_edit_details(int64_t timeEntryID)
{
	setTimeEntryID(timeEntryID);
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
	m_timeEntryID = 0;
	m_taskID = taskID;
	rangeStart = startTime;
	rangeStop = stopTime;

	on_task_name_updated(taskID);
	on_task_total_time_updated(taskID);

	detailList.set(taskID, rangeStart, rangeStop);
	checkForChanges();
	on_runningTasksChanged();
}

void DetailsDialog::setTimeEntryID(int64_t timeEntryID)
{
	auto te = m_timeAccessor.by_ID(timeEntryID);
	if (te)
	{
		oldStartTime = te->start();
		startTimeHour.set_sensitive(true);
		startTimeMinute.set_sensitive(true);
		if (te->running())
		{
			stopTimeHour.set_sensitive(false);
			stopTimeMinute.set_sensitive(false);
		}
		else
		{
			stopTimeHour.set_sensitive(true);
			stopTimeMinute.set_sensitive(true);
			oldStopTime = te->stop();
		}
		m_timeEntryID = timeEntryID;
		setValues();
	}
	else
	{
		startTimeHour.set_sensitive(false);
		startTimeMinute.set_sensitive(false);
		stopTimeHour.set_sensitive(false);
		stopTimeMinute.set_sensitive(false);
	}
}

void DetailsDialog::checkForChanges()
{
	if (m_timeEntryID > 0 && (oldStartTime != startTime || oldStopTime != stopTime))
	{
		OKButton.set_sensitive(true);
		CancelButton.set_sensitive(true);
	}
	else
	{
		OKButton.set_sensitive(false);
		CancelButton.set_sensitive(false);
	}
}

// also displays whether idle
void DetailsDialog::on_runningTasksChanged()
{
	std::vector<int64_t> taskIDs = m_timeAccessor.currently_running();
	bool runningThisTaskID = false;
	if (taskIDs.size() > 0)
	{
		set_title("TimeIT ⌚");
		for (auto taskID : taskIDs)
		{
			if (taskID == m_taskID)
			{
				runningThisTaskID = true;
				break;
			}
		}
	}
	else
	{
		set_title("TimeIT");
	}
	if (runningThisTaskID)
	{
		if (!m_timeKeeper.isIdle())
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

void DetailsDialog::on_task_name_updated(int64_t task_id)
{
	if (task_id == m_taskID)
	{
		auto task = m_taskAccessor.by_ID(m_taskID);
		if (task.has_value())
		{
			taskName.set_text(task->name());
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

void DetailsDialog::on_task_total_time_updated(int64_t task_id)
{
	if (task_id == m_taskID)
	{
		if (difftime(rangeStop, rangeStart) > 86403)
		{
			// longer than a day, could be a week, month, year, with a margin to stay clear of leap seconds
			auto task = m_taskAccessor.by_ID(m_taskID);
			if (task.has_value())
			{
				time_t total_time = m_timeAccessor.total_cumulative_time(m_taskID, rangeStart, rangeStop);
				taskTotalTime.set_text("∑ = " + libtimeit::seconds2hhmm(total_time));
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
