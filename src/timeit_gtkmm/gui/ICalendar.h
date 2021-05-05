/*
 * ICalendar.h
 *
 *  Created on: Jan 17, 2009
 *      Author: hoglet
 */

#ifndef ICALENDAR_H_
#define ICALENDAR_H_

namespace gui
{

class CalendarObserver
{
public:
	virtual ~CalendarObserver() {};
	virtual void on_date_changed() = 0;
};
class ICalendar
{
public:
	virtual ~ICalendar() {};
	virtual void attach(CalendarObserver*) = 0;
	virtual void detach(CalendarObserver*) = 0;
	virtual void get_date(unsigned int& year, unsigned int& month, unsigned int& day) const = 0;
};
}
#endif /* ICALENDAR_H_ */
