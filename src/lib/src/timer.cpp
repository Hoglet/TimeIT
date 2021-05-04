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
Timer_observer::Timer_observer(Timer& publisher) : timer(publisher )
{
	publisher.attach(this);
}


Timer_observer::~Timer_observer()
{
	timer.detach(this);
}

void Timer_observer::on_signal_1_second()
{

}

void Timer_observer::on_signal_10_seconds()
{

}
//LCOV_EXCL_STOP


void Timer::attach(Timer_observer *observer)
{
	observers.push_back(observer);
}

void Timer::detach(Timer_observer *observer)
{
	observers.remove(observer);
}

bool Timer::on_signal_1_second()
{
	signalSender();
	return true;
}

//This function is called every second
void Timer::signalSender()
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
