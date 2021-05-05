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
#include "ICalendar.h"

namespace gui
{

class Calendar : public Gtk::Calendar, public ICalendar
{
public:
	Calendar();
	void attach(CalendarObserver* observer) override;
	void detach(CalendarObserver* observer) override;
	void get_date(unsigned int& year, unsigned int& month, unsigned int& day) const override;
private:
	void notify();
	std::list<CalendarObserver*> observers;
};

}
#endif /* CALENDAR_H_ */
