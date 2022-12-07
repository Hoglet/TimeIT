/*
 * Timer.cpp
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */

#include "libtimeit/timer.h"

namespace libtimeit
{


//LCOV_EXCL_START
timer_observer::timer_observer( timer_base& publisher) : timer( publisher )
{
	publisher.attach(this);
}


timer_observer::~timer_observer()
{
	timer.detach(this);
}

void timer_observer::on_signal_1_second()
{

}

void timer_observer::on_signal_10_seconds()
{

}
//LCOV_EXCL_STOP


void timer_base::attach( timer_observer *observer)
{
	observers.push_back(observer);
}

void timer_base::detach( timer_observer *observer)
{
	observers.remove(observer);
}

bool timer_base::on_signal_1_second()
{
	signal_sender();
	return true;
}

//This function is called every second
void timer_base::signal_sender()
{
	ten_second_counter--;
	bool signal_10_seconds = false;
	if (ten_second_counter < 1)
	{
		signal_10_seconds = true;
		ten_second_counter = 10;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	}
	for (auto* observer : observers)
	{
		observer->on_signal_1_second();
		if (signal_10_seconds)
		{
			observer->on_signal_10_seconds();
		}
	}

}

}
