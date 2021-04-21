/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARYVIEWS_H_
#define SUMMARYVIEWS_H_

#include <gtkmm.h>
#include "summary.h"
#include "libtimeit/utils.h"

namespace GUI
{
using namespace std;
using namespace libtimeit;

class DaySummary: public Summary
{
public:
	DaySummary( Database& database, Notifier& notifier) :
		Summary( database, notifier)
	{
		init();
	}
	;
	virtual ~DaySummary()
	{
	}
	;
private:
	virtual void calculateTimeSpan()
	{
		startTime = libtimeit::beginning_of_day(activeDay);
		stopTime = libtimeit::end_of_day(activeDay);
	}
};

class WeekSummary: public Summary
{
public:
	WeekSummary( Database& database, Notifier& notifier) :
		Summary( database, notifier)
	{
		init();
	}
	;
	virtual ~WeekSummary()
	{
	}
	;
private:
	virtual void calculateTimeSpan()
	{
		startTime = libtimeit::beginning_of_week(activeDay);
		stopTime = libtimeit::end_of_week(activeDay);
	}
};

class MonthSummary: public Summary
{
public:
	MonthSummary( Database& database, Notifier& notifier) :
		Summary( database, notifier)
	{
		init();
	}
	;
	virtual ~MonthSummary()
	{
	}
	;
private:
	virtual void calculateTimeSpan()
	{
		startTime = libtimeit::beginning_of_month(activeDay);
		stopTime = libtimeit::end_of_month(activeDay);
	}
};

class YearSummary: public Summary
{
public:
	YearSummary( Database& database, Notifier& notifier) :
		Summary( database, notifier)
	{
		init();
	}
	;
	virtual ~YearSummary()
	{
	}
	;
private:
	virtual void calculateTimeSpan()
	{
		startTime = libtimeit::beginning_of_year(activeDay);
		stopTime = libtimeit::end_of_year(activeDay);
	}
};

}
#endif /* DAYSUMMARY_H_ */
