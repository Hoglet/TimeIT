/*
 * Calendar.h
 *
 *  Created on: 2008-nov-22
 *      Author: hoglet
 */

#ifndef CALENDAR_H_
#define CALENDAR_H_

#include <gtkmm.h>
#include <list>

namespace gui
{

class CalendarObserver
{
public:
	virtual void on_date_changed() = 0;
};

class Calendar : public Gtk::Calendar
{
public:
	Calendar();
	void attach(CalendarObserver* observer);
	void detach(CalendarObserver* observer);
private:
	void notify();
	void mark_today();

	int marked_day=0;
	std::list<CalendarObserver*> observers;
};

}
#endif /* CALENDAR_H_ */
