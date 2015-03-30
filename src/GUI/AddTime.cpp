/*
 * AddTime.cpp
 *
 *  Created on: 2008-okt-26
 *      Author: hoglet
 */

#include "AddTime.h"
#include <Utils.h>
#include <ExtendedTask.h>
#include <time.h>
#include <glibmm/i18n.h>

using namespace DB;

namespace GUI
{

AddTime::AddTime(int64_t op_taskID, ICalendar& op_calendar, std::shared_ptr<DB::IDatabase>& database) :
		table(4, 4), yearLabel(_("Year")), monthLabel(_("Month")), dayLabel(_("Day")), taskNameLabel(
				_("Adding time to:")), startTimeLabel(_("Start time")), stopTimeLabel(_("Stop time")), calendar(
				op_calendar), taskID(op_taskID), m_timeAccessor(database->getTimeAccessor()), m_taskAccessor(database->getTaskAccessor())
{

	set_deletable(false);
	//OKButton.set_sensitive(false);

	std::shared_ptr<Task> task = m_taskAccessor->getTask(taskID);
	if (task)
	{
		taskName.set_text(task->getName());
	}
	startTimeHour.set_range(0, 23);
	startTimeMinute.set_range(0, 59);
	stopTimeHour.set_range(0, 23);
	stopTimeMinute.set_range(0, 59);
	year.set_range(2000, 2050);
	month.set_range(1, 12);
	day.set_range(1, 31);
	year.set_increments(1, 1);
	month.set_increments(1, 1);
	day.set_increments(1, 1);
	startTimeHour.set_increments(1, 1);
	startTimeMinute.set_increments(1, 1);
	stopTimeHour.set_increments(1, 1);
	stopTimeMinute.set_increments(1, 1);

	time_t now = Utils::now();
	struct tm* timeInfo = localtime(&now);
	startTimeHour.set_value(timeInfo->tm_hour);
	stopTimeHour.set_value(timeInfo->tm_hour);
	startTimeMinute.set_value(timeInfo->tm_min);
	stopTimeMinute.set_value(timeInfo->tm_min);

	calendar.attach(this);
	startTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &AddTime::on_change));
	startTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &AddTime::on_change));
	stopTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &AddTime::on_change));
	stopTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &AddTime::on_change));
	month.signal_value_changed().connect(sigc::mem_fun(this, &AddTime::on_month_changed));

	on_date_changed();
	//Layout
	set_resizable(false);
	get_vbox()->set_spacing(2);
	get_vbox()->pack_start(hbox);
	{
		hbox.pack_start(taskNameLabel);
		hbox.pack_start(taskName);
		hbox.set_spacing(2);
	}
	get_vbox()->pack_start(hseparator);
	get_vbox()->pack_start(table, Gtk::PACK_SHRINK, 3);
	{
		table.attach(yearLabel, 0, 2, 0, 1);
		table.attach(monthLabel, 2, 3, 0, 1);
		table.attach(dayLabel, 3, 4, 0, 1);
		table.attach(year, 0, 2, 1, 2);
		table.attach(month, 2, 3, 1, 2);
		table.attach(day, 3, 4, 1, 2);
		table.attach(startTimeLabel, 0, 2, 2, 3);
		table.attach(stopTimeLabel, 2, 4, 2, 3);
		table.attach(startTimeHour, 0, 1, 3, 4);
		table.attach(startTimeMinute, 1, 2, 3, 4);
		table.attach(stopTimeHour, 2, 3, 3, 4);
		table.attach(stopTimeMinute, 3, 4, 3, 4);
	}
	//get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	//get_action_area()->pack_start(CancelButton);
	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	OKButton = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	OKButton->set_sensitive(false);
	show_all_children();

	//Connect signals
	/*	OKButton.signal_clicked().connect(sigc::mem_fun(this, &DetailsDialog::on_OKButton_clicked));
	 CancelButton.signal_clicked().connect(sigc::mem_fun(this, &DetailsDialog::on_CancelButton_clicked));
	 startTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	 startTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	 stopTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	 stopTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	 */

}

AddTime::~AddTime()
{
	calendar.detach(this);
}

void AddTime::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		guint y = year.get_value_as_int();
		guint m = month.get_value_as_int() - 1;
		guint d = day.get_value_as_int();

		int startH = startTimeHour.get_value_as_int();
		int startM = startTimeMinute.get_value_as_int();
		int stopH = stopTimeHour.get_value_as_int();
		int stopM = stopTimeMinute.get_value_as_int();

		time_t startTime = Utils::getTime(y, m, d, startH, startM);
		time_t stopTime = Utils::getTime(y, m, d, stopH, stopM);
		m_timeAccessor->newTime(taskID, startTime, stopTime);
	}
}

void AddTime::on_date_changed()
{
	guint l_year;
	guint l_month;
	guint l_day;
	calendar.get_date(l_year, l_month, l_day);

	year.set_value(l_year);
	month.set_value(l_month + 1);
	day.set_value(l_day);
}

void AddTime::on_change()
{
	int startH = startTimeHour.get_value_as_int();
	int stopH = stopTimeHour.get_value_as_int();
	int startM = startTimeMinute.get_value_as_int();
	int stopM = stopTimeMinute.get_value_as_int();
	if (stopH > startH || (stopH == startH && stopM > startM))
	{
		OKButton->set_sensitive(true);
	} else
	{
		OKButton->set_sensitive(false);
	}
}

void AddTime::on_month_changed()
{

	guint y = year.get_value_as_int();
	guint m = month.get_value_as_int() - 1;

	//Avoiding problems with dayligt saving time by checking second day of month
	time_t activeDay = Utils::getTime(y, m, 2);
	int maxDay = Utils::getDaysInMonth(activeDay);
	day.set_range(1, maxDay);
}

}
