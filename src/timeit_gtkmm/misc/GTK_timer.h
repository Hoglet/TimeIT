//
// Created by hoglet on 03/04/2021.
//

#ifndef TIMEIT_GTK_TIMER_H
#define TIMEIT_GTK_TIMER_H
#include <libtimeit/timer.h>

namespace gui
{
using sigc::connection;
using sigc::mem_fun;
using namespace Glib;

class GTK_timer : public libtimeit::Timer
{
public:
	GTK_timer()
	{
		conn = signal_timeout().connect_seconds(
				mem_fun(this, &Timer::on_signal_1_second),
				1);
	}
	GTK_timer(const GTK_timer&) = delete;
	GTK_timer( GTK_timer&& ) = delete;
	GTK_timer& operator=( const GTK_timer& ) = delete;
	GTK_timer& operator=( GTK_timer&& ) = delete;

private:
	connection conn{};
};

}
#endif //TIMEIT_GTK_TIMER_H
