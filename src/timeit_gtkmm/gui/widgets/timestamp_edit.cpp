#include <libtimeit/utils.h>
#include "timestamp_edit.h"


namespace gui
{

static const int YEAR_ZERO = 1900;

timestamp_edit::timestamp_edit(string title)
{
	set_label(title);
	set_behaviour();
	connect_signals();
	do_layout();


}
void timestamp_edit::set_values(time_t timestamp)
{
	struct tm *time_info = localtime(&timestamp);
	hour.set_value(time_info->tm_hour);
	minute.set_value(time_info->tm_min);

	year.set_value(time_info->tm_year + YEAR_ZERO);
	month.set_value(time_info->tm_mon + 1);
	day.set_value(time_info->tm_mday);
}

void timestamp_edit::connect_signals()
{
	auto slot =
			[this]()
			{
				this->on_change();
			};
	year.signal_changed().connect(slot);
	month.signal_changed().connect(slot);
	day.signal_changed().connect(slot);
	hour.signal_changed().connect(slot);
	minute.signal_changed().connect(slot);
}

void timestamp_edit::do_layout()
{
	set_border_width(7);
	add(padding_box);
	padding_box.pack_start(vertical_box,true,false,10);
	vertical_box.set_spacing(2);
	vertical_box.pack_start(date_table, true, false,10);
	{
		date_table.set_spacings(5);
		date_table.attach(year_label, 0, 2, 0, 1);
		date_table.attach(month_label, 2, 4, 0, 1);
		date_table.attach( day_label, 4, 7, 0, 1); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		date_table.attach(year, 0, 2, 1, 2);
		date_table.attach(month, 2, 4, 1, 2);
		date_table.attach(day, 4, 7, 1, 2);       // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	}

	vertical_box.pack_end(time_box, true, false, 7);
	{
		time_box.set_spacing(7);
		time_box.pack_start(time_label, Gtk::PACK_SHRINK);
		time_box.pack_start(hour, Gtk::PACK_SHRINK);
		time_box.pack_start(colon_label, Gtk::PACK_SHRINK);
		time_box.pack_start(minute, Gtk::PACK_SHRINK);
	}
}

void timestamp_edit::set_behaviour()
{
	hour.set_range(0, 23);             // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	hour.set_increments(1, 1);
	minute.set_range(0, 59);           // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	minute.set_increments(1, 1);

	year.set_range(2000, 2050);        // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	month.set_range(1, 12);            // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	day.set_range(1, 31);              // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	year.set_increments(1, 1);
	month.set_increments(1, 1);
	day.set_increments(1, 1);
}

void timestamp_edit::on_change()
{
	signal_changed.emit();
}

time_t timestamp_edit::timestamp()
{
	auto y = year.get_value_as_int();
	auto m = month.get_value_as_int() - 1;
	auto d = day.get_value_as_int();

	//Avoiding problems with daylight saving time by checking second day of month
	time_t active_day = libtimeit::to_time(y, m, 2);
	int max_day = libtimeit::days_in_month(active_day);
	day.set_range(1, max_day);

	int h = hour.get_value_as_int();
	int mm = minute.get_value_as_int();

	return libtimeit::to_time(y, m, d, h, mm);
}

}