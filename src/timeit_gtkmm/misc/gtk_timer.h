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

class gtk_timer : public libtimeit::Timer
{
public:
	gtk_timer()
	{
		conn = signal_timeout().connect_seconds(
				[this]() -> bool
				{
					return this->on_signal_1_second();
				},
				1);
	}
	gtk_timer(const gtk_timer&) = delete;
	gtk_timer( gtk_timer&& ) = delete;
	gtk_timer& operator=( const gtk_timer& ) = delete;
	gtk_timer& operator=( gtk_timer&& ) = delete;
	~gtk_timer() = default;
private:
	connection conn{};
};

}
#endif //TIMEIT_GTK_TIMER_H
