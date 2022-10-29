#include "calendar.h"
#include <libtimeit/utils.h>

namespace gui
{
using namespace libtimeit;

calendar_widget::calendar_widget()
{
	signal_day_selected().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_day_selected_double_click().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_month_changed().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_next_month().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_next_year().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_prev_month().connect(sigc::mem_fun(this, &calendar_widget::notify));
	signal_prev_year().connect(sigc::mem_fun(this, &calendar_widget::notify));
	mark_today();

}

void calendar_widget::notify()
{
	for (auto* observer: observers)
	{
		observer->on_date_changed();
	}
	mark_today();
}

void calendar_widget::attach(calendar_observer* observer)
{
	observers.push_back(observer);
}

void calendar_widget::detach(calendar_observer* observer)
{
	observers.remove(observer);
}

void calendar_widget::mark_today()
{
	auto now = system_clock::now();

	guint year{0};
	guint month{0};
	guint day{0};
	get_date(year, month, day);
	struct tm today = localtime( now );
	int current_year = today.tm_year + 1900;
	int current_month = today.tm_mon;

	unmark_day(marked_day);
	if (
			current_year == (int) year &&
			current_month == (int) month
			)
	{
		marked_day = today.tm_mday;
		mark_day(marked_day);
	}
}

}
