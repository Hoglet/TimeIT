/*
 * AddTime.cpp
 *
 *  Created on: 2008-okt-26
 *      Author: hoglet
 */

#include "add_time.h"
#include <libtimeit/utils.h>
#include <ctime>
#include <glibmm/i18n.h>

using namespace libtimeit;

namespace gui
{

Edit_time::Edit_time(
		int64_t    op_taskID,
		Calendar& op_calendar,
		Database& database)
		:
		year_label(_("Year")),
		month_label(_("Month")),
		day_label(_("Day")),
		task_name_label(_("Adding time to:")),
		start_time_label(_("Start time")),
		stop_time_label(_("Stop time")),
		start_colon_label(":"), to_label("→"),
		stop_colon_label(":"),
		calendar(op_calendar), task_id(op_taskID),
		time_accessor(database),
		task_accessor(database)
{
	set_deletable(false);

	auto task = task_accessor.by_ID(task_id);
	if (task.has_value())
	{
		task_name.set_text(task->name);
	}
	start_time_hour.set_range(0, 23);
	start_time_minute.set_range(0, 59);
	stop_time_hour.set_range(0, 23);
	stop_time_minute.set_range(0, 59);
	year.set_range(2000, 2050);
	month.set_range(1, 12);
	day.set_range(1, 31);
	year.set_increments(1, 1);
	month.set_increments(1, 1);
	day.set_increments(1, 1);
	start_time_hour.set_increments(1, 1);
	start_time_minute.set_increments(1, 1);
	stop_time_hour.set_increments(1, 1);
	stop_time_minute.set_increments(1, 1);

	time_t now = libtimeit::now();
	struct tm *time_info = localtime(&now);
	start_time_hour.set_value(time_info->tm_hour);
	stop_time_hour.set_value(time_info->tm_hour);
	start_time_minute.set_value(time_info->tm_min);
	stop_time_minute.set_value(time_info->tm_min);

	calendar.attach(this);
	start_time_hour.signal_value_changed().connect(sigc::mem_fun(this, &Edit_time::on_change));
	start_time_minute.signal_value_changed().connect(sigc::mem_fun(this, &Edit_time::on_change));
	stop_time_hour.signal_value_changed().connect(sigc::mem_fun(this, &Edit_time::on_change));
	stop_time_minute.signal_value_changed().connect(sigc::mem_fun(this, &Edit_time::on_change));
	month.signal_value_changed().connect(sigc::mem_fun(this, &Edit_time::on_month_changed));

	on_date_changed();
	//Layout
	set_resizable(false);
	get_vbox()->set_spacing(2);
	get_vbox()->pack_start(hbox);
	{
		hbox.pack_start(task_name_label);
		hbox.pack_start(task_name);
		hbox.set_spacing(2);
	}
	get_vbox()->pack_start(hseparator);
	get_vbox()->pack_start(table, Gtk::PACK_SHRINK, 3);
	{
 		table.attach(year_label, 0, 2, 0, 1);
		table.attach(month_label, 2, 4, 0, 1);
		table.attach(day_label, 4, 7, 0, 1);
 		table.attach(year, 0, 2, 1, 2);
		table.attach(month, 2, 4, 1, 2);
		table.attach(day, 4, 7, 1, 2);
		table.attach(start_time_label, 0, 3, 2, 3);
		table.attach(stop_time_label, 4, 7, 2, 3);
 		table.attach(start_time_hour, 0, 1, 3, 4);
		table.attach(start_colon_label, 1, 2, 3, 4);
		table.attach(start_time_minute, 2, 3, 3, 4);
		table.attach(to_label, 3, 4, 3, 4);
		table.attach(stop_time_hour, 4, 5, 3, 4);
		table.attach(stop_colon_label, 5, 6, 3, 4);
		table.attach(stop_time_minute, 6, 7, 3, 4);
	}

	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	ok_button = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	ok_button->set_sensitive(false);
	show_all_children();


}

Edit_time::~Edit_time()
{
	calendar.detach(this);
}

void Edit_time::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		auto y = year.get_value_as_int();
		auto m = month.get_value_as_int() - 1;
		auto d = day.get_value_as_int();

		int start_h = start_time_hour.get_value_as_int();
		int start_m = start_time_minute.get_value_as_int();
		int stop_h = stop_time_hour.get_value_as_int();
		int stop_m = stop_time_minute.get_value_as_int();

		time_t start_time = libtimeit::to_time(y, m, d, start_h, start_m);
		time_t stop_time = libtimeit::to_time(y, m, d, stop_h, stop_m);
		time_accessor.create(Time_entry(task_id, start_time, stop_time) );
	}
}

void Edit_time::on_date_changed()
{
	guint year_{0};
	guint month_{0};
	guint day_{0};
	calendar.get_date(year_, month_, day_);

	year.set_value(year_);
	month.set_value(month_ + 1);
	day.set_value(day_);
}

void Edit_time::on_change()
{
	int start_h = start_time_hour.get_value_as_int();
	int stop_h = stop_time_hour.get_value_as_int();
	int start_m = start_time_minute.get_value_as_int();
	int stop_m = stop_time_minute.get_value_as_int();
	if (stop_h > start_h || (stop_h == start_h && stop_m > start_m))
	{
		ok_button->set_sensitive(true);
	}
	else
	{
		ok_button->set_sensitive(false);
	}
}

void Edit_time::on_month_changed()
{

	auto y = year.get_value_as_int();
	auto m = month.get_value_as_int() - 1;

	//Avoiding problems with daylight saving time by checking second day of month
	time_t active_day = libtimeit::to_time(y, m, 2);
	int max_day = libtimeit::days_in_month(active_day);
	day.set_range(1, max_day);
}

}

