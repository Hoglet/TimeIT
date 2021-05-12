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
	void calculate_time_span() override
	{
		start_time = libtimeit::beginning_of_day(active_day);
		stop_time = libtimeit::end_of_day(active_day);
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
	void calculate_time_span() override
	{
		start_time = libtimeit::beginning_of_week(active_day);
		stop_time = libtimeit::end_of_week(active_day);
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
	void calculate_time_span() override
	{
		start_time = libtimeit::beginning_of_month(active_day);
		stop_time = libtimeit::end_of_month(active_day);
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
	void calculate_time_span() override
	{
		start_time = libtimeit::beginning_of_year(active_day);
		stop_time = libtimeit::end_of_year(active_day);
	}
};

}
#endif /* SUMMARY_VIEWS_H_ */
