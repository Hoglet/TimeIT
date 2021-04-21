/*
 * Calendar.cpp
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#include "calendar.h"

namespace GUI
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
}

Calendar::~Calendar()
{
}

void Calendar::get_date(unsigned int& year, unsigned int& month, unsigned int& day) const
{
	Gtk::Calendar::get_date(year, month, day);
}

void Calendar::notify()
{
	std::list<CalendarObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		CalendarObserver* observer = *iter;
		observer->on_date_changed();
	}
}

void Calendar::attach(CalendarObserver* observer)
{
	observers.push_back(observer);
}
void Calendar::detach(CalendarObserver* observer)
{
	observers.remove(observer);
}

}
