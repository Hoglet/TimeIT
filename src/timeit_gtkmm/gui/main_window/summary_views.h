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

class day_summary_widget : public summary
{
public:
	day_summary_widget(database &database, notification_manager &notifier) :
			summary(database, notifier)
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

class week_summary_widget : public summary
{
public:
	week_summary_widget(database &database, notification_manager &notifier) :
			summary(database, notifier)
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

class month_summary_widget : public summary
{
public:
	month_summary_widget(database &database, notification_manager &notifier) :
			summary(database, notifier)
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

class year_summary_widget : public summary
{
public:
	year_summary_widget(database &database, notification_manager &notifier) :
			summary(database, notifier)
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
