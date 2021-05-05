/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARY_VIEWS_H_
#define SUMMARY_VIEWS_H_

#include <gtkmm.h>
#include "summary.h"
#include "libtimeit/utils.h"

namespace gui
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
	};
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
	};
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
	};
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
	};
private:
	virtual void calculateTimeSpan()
	{
		startTime = libtimeit::beginning_of_year(activeDay);
		stopTime = libtimeit::end_of_year(activeDay);
	}
};

}
#endif /* SUMMARY_VIEWS_H_ */
