#include "calendar.h"
#include <libtimeit/utils.h>

namespace gui
{
using namespace libtimeit;

calendar_widget::calendar_widget( )
{
	mark_today( );
	signal_next_month( ).connect( [ & ] { this->mark_today( ); } );
	signal_prev_month( ).connect( [ & ] { this->mark_today( ); } );
	signal_next_year( ).connect( [ & ] { this->mark_today( ); } );
	signal_prev_year( ).connect( [ & ] { this->mark_today( ); } );
	Glib::signal_timeout( ).connect_seconds(
			[ & ]( ) -> bool
			{
				this->mark_today( );
				return true;
			}, 300 );
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
