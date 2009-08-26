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
#include "Utils.h"

namespace GUI
{
namespace Internal
{

class DaySummary: public Summary
{
public:
	DaySummary( boost::shared_ptr<DB::Database>& database) :
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
		startTime = Utils::getBeginingOfDay(activeDay);
		stopTime = Utils::getEndOfDay(activeDay);
	}
};

class WeekSummary: public Summary
{
public:
	WeekSummary( boost::shared_ptr<DB::Database>& database) :
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
		startTime = Utils::getBeginingOfWeek(activeDay);
		stopTime = Utils::getEndOfWeek(activeDay);
	}
};

class MonthSummary: public Summary
{
public:
	MonthSummary( boost::shared_ptr<DB::Database>& database) :
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
		startTime = Utils::getBeginingOfMonth(activeDay);
		stopTime = Utils::getEndOfMonth(activeDay);
	}
};

}
}
#endif /* DAYSUMMARY_H_ */
