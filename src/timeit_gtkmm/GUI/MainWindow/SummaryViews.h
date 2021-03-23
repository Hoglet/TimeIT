/*
 * DaySummary.h
 *
 *  Created on: 2008-jul-12
 *      Author: hoglet
 */

#ifndef SUMMARYVIEWS_H_
#define SUMMARYVIEWS_H_

#include <gtkmm.h>
#include "Summary.h"
#include "libtimeit/Utils.h"

namespace GUI
{
using namespace std;
using namespace libtimeit;

class DaySummary: public Summary
{
public:
	DaySummary( Database& database) :
		Summary( database)
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
		startTime = libtimeit::getBeginingOfDay(activeDay);
		stopTime = libtimeit::getEndOfDay(activeDay);
	}
};

class WeekSummary: public Summary
{
public:
	WeekSummary( Database& database) :
		Summary( database)
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
		startTime = libtimeit::getBeginingOfWeek(activeDay);
		stopTime = libtimeit::getEndOfWeek(activeDay);
	}
};

class MonthSummary: public Summary
{
public:
	MonthSummary( Database& database) :
		Summary( database)
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
		startTime = libtimeit::getBeginingOfMonth(activeDay);
		stopTime = libtimeit::getEndOfMonth(activeDay);
	}
};

class YearSummary: public Summary
{
public:
	YearSummary( Database& database) :
		Summary( database)
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
		startTime = libtimeit::getBeginingOfYear(activeDay);
		stopTime = libtimeit::getEndOfYear(activeDay);
	}
};

}
#endif /* DAYSUMMARY_H_ */
