#include <glibmm/main.h>
#include "gtk_timer.h"

namespace gui
{

gtk_timer::gtk_timer()
{
	conn = Glib::signal_timeout().connect_seconds(
			[this]() -> bool
			{
				return this->on_signal_1_second();
			},
			1);
}

}