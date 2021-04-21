/*
 * AddTime.cpp
 *
 *  Created on: 2008-okt-26
 *      Author: hoglet
 */

#include "add_time.h"
#include <libtimeit/utils.h>
#include <time.h>
#include <glibmm/i18n.h>

using namespace libtimeit;

namespace GUI
{

AddTime::AddTime(
		int64_t    op_taskID,
		ICalendar& op_calendar,
		Database& database)
		:
		yearLabel(_("Year")),
		monthLabel(_("Month")),
		dayLabel(_("Day")),
		taskNameLabel(_("Adding time to:")),
		startTimeLabel(_("Start time")),
		stopTimeLabel(_("Stop time")),
		startColonLabel(":"), toLabel("→"),
		stopColonLabel(":"),
		calendar(op_calendar), taskID(op_taskID),
		m_timeAccessor(database),
		m_taskAccessor(database)
{
	set_deletable(false);

	auto task = m_taskAccessor.by_ID(taskID);
	if (task.has_value())
	{
		taskName.set_text(task->name);
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

	time_t now = libtimeit::now();
	struct tm *timeInfo = localtime(&now);
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
		table.attach(monthLabel, 2, 4, 0, 1);
		table.attach(dayLabel, 4, 7, 0, 1);
 		table.attach(year, 0, 2, 1, 2);
		table.attach(month, 2, 4, 1, 2);
		table.attach(day, 4, 7, 1, 2);
		table.attach(startTimeLabel, 0, 3, 2, 3);
		table.attach(stopTimeLabel, 4, 7, 2, 3);
 		table.attach(startTimeHour, 0, 1, 3, 4);
		table.attach(startColonLabel, 1, 2, 3, 4);
		table.attach(startTimeMinute, 2, 3, 3, 4);
		table.attach(toLabel, 3, 4, 3, 4);
		table.attach(stopTimeHour, 4, 5, 3, 4);
		table.attach(stopColonLabel, 5, 6, 3, 4);
		table.attach(stopTimeMinute, 6, 7, 3, 4);
	}

	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	OKButton = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	OKButton->set_sensitive(false);
	show_all_children();


}

AddTime::~AddTime()
{
	calendar.detach(this);
}

void AddTime::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		auto y = year.get_value_as_int();
		auto m = month.get_value_as_int() - 1;
		auto d = day.get_value_as_int();

		int startH = startTimeHour.get_value_as_int();
		int startM = startTimeMinute.get_value_as_int();
		int stopH = stopTimeHour.get_value_as_int();
		int stopM = stopTimeMinute.get_value_as_int();

		time_t startTime = libtimeit::to_time(y, m, d, startH, startM);
		time_t stopTime = libtimeit::to_time(y, m, d, stopH, stopM);
		m_timeAccessor.create( Time_entry(taskID, startTime, stopTime) );
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
	}
	else
	{
		OKButton->set_sensitive(false);
	}
}

void AddTime::on_month_changed()
{

	auto y = year.get_value_as_int();
	auto m = month.get_value_as_int() - 1;

	//Avoiding problems with daylight saving time by checking second day of month
	time_t activeDay = libtimeit::to_time(y, m, 2);
	int maxDay = libtimeit::days_in_month(activeDay);
	day.set_range(1, maxDay);
}

}

