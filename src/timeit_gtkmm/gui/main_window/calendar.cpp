/*
 * Calendar.cpp
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#include "calendar.h"
#include <libtimeit/utils.h>
namespace gui
{

Calendar::Calendar()
{
	signal_day_selected().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_day_selected_double_click().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_month_changed().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_next_month().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_next_year().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_prev_month().connect(sigc::mem_fun(this, &Calendar::notify));
	signal_prev_year().connect(sigc::mem_fun(this, &Calendar::notify));
	mark_today();

}

void Calendar::notify()
{
	for (auto* observer: observers)
	{
		observer->on_date_changed();
	}
	mark_today();
}

void Calendar::attach(CalendarObserver* observer)
{
	observers.push_back(observer);
}
void Calendar::detach(CalendarObserver* observer)
{
	observers.remove(observer);
}

void Calendar::mark_today()
{
	auto now = libtimeit::now();

	guint year{0};
	guint month{0};
	guint day{0};
	get_date(year, month, day);
	struct tm* today  = localtime(&now);
	int current_year  = today->tm_year + 1900;
	int current_month = today->tm_mon;

	unmark_day(marked_day);
	if(
			current_year  == (int)year   &&
			current_month == (int)month
			)
	{
		marked_day=today->tm_mday;
		mark_day(marked_day);
	}
}

}
