#include "edit_time.h"
#include <libtimeit/utils.h>
#include <ctime>
#include <utility>
#include <glibmm/i18n.h>

using namespace libtimeit;

namespace gui
{

Edit_time::Edit_time(
		Time_entry time_entry_,
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
		time_entry(std::move(time_entry_)),
		time_accessor(database),
		task_accessor(database)
{
	set_deletable(false);

	auto task = task_accessor.by_ID(time_entry.task_id);
	if (task.has_value())
	{
		task_name.set_text(task->name);
	}
	start_time_hour.set_range(0, 23);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	start_time_minute.set_range(0, 59);// NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	stop_time_hour.set_range(0, 23);   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	stop_time_minute.set_range(0, 59); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	year.set_range(2000, 2050);        // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	month.set_range(1, 12);            // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	day.set_range(1, 31);              // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	year.set_increments(1, 1);
	month.set_increments(1, 1);
	day.set_increments(1, 1);
	start_time_hour.set_increments(1, 1);
	start_time_minute.set_increments(1, 1);
	stop_time_hour.set_increments(1, 1);
	stop_time_minute.set_increments(1, 1);

	vector<Gtk::SpinButton*> widgets{};
	widgets.push_back(&year);
	widgets.push_back(&month);
	widgets.push_back(&day);
	widgets.push_back(&start_time_hour);
	widgets.push_back(&start_time_minute);
	widgets.push_back(&stop_time_hour);
	widgets.push_back(&stop_time_minute);

	set_values();

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
		table.attach(day_label, 4, 7, 0, 1);         // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
 		table.attach(year, 0, 2, 1, 2);
		table.attach(month, 2, 4, 1, 2);
		table.attach(day, 4, 7, 1, 2);               // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		table.attach(start_time_label, 0, 3, 2, 3);
		table.attach(stop_time_label, 4, 7, 2, 3);   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
 		table.attach(start_time_hour, 0, 1, 3, 4);
		table.attach(start_colon_label, 1, 2, 3, 4);
		table.attach(start_time_minute, 2, 3, 3, 4);
		table.attach(to_label, 3, 4, 3, 4);
		table.attach(stop_time_hour, 4, 5, 3, 4);    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		table.attach(stop_colon_label, 5, 6, 3, 4);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		table.attach(stop_time_minute, 6, 7, 3, 4);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	}

	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	ok_button = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	ok_button->set_sensitive(false);

	connect_signals(widgets);

	show_all_children();


}

void Edit_time::connect_signals(vector<Gtk::SpinButton*> widgets)
{
	for( auto* widget: widgets)
	{
		if(widget!=nullptr)
		{
			widget->signal_changed().connect(
					[this]()
					{
						this->on_change();
					});
		}
	}
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
		if(time_entry.id>0)
		{
			time_accessor.update(time_entry.with_start(start_time).with_stop(stop_time));
		}
		else
		{
			time_accessor.create(time_entry.with_start(start_time).with_stop(stop_time));
		}
	}
}

static const int YEAR_ZERO = 1900;

void Edit_time::set_values()
{

	struct tm *time_info = localtime(&time_entry.start);
	start_time_hour.set_value(time_info->tm_hour);
	start_time_minute.set_value(time_info->tm_min);

	year.set_value(time_info->tm_year + YEAR_ZERO);
	month.set_value(time_info->tm_mon + 1);
	day.set_value(time_info->tm_mday);

	time_info = localtime(&time_entry.stop);
	stop_time_hour.set_value(time_info->tm_hour);
	stop_time_minute.set_value(time_info->tm_min);

}

void Edit_time::on_change()
{
	auto y = year.get_value_as_int();
	auto m = month.get_value_as_int() - 1;

	//Avoiding problems with daylight saving time by checking second day of month
	time_t active_day = libtimeit::to_time(y, m, 2);
	int max_day = libtimeit::days_in_month(active_day);
	day.set_range(1, max_day);

	int start_h = start_time_hour.get_value_as_int();
	int stop_h = stop_time_hour.get_value_as_int();
	int start_m = start_time_minute.get_value_as_int();
	int stop_m = stop_time_minute.get_value_as_int();
	if (
		stop_h > start_h ||
		(stop_h == start_h && stop_m > start_m)
		)
	{
		ok_button->set_sensitive(true);
	}
	else
	{
		ok_button->set_sensitive(false);
	}
}

}

