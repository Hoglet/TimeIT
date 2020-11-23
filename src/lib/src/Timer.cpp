/*
 * Timer.cpp
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */

#include "libtimeit/Timer.h"

namespace libtimeit
{


//LCOV_EXCL_START
TimerObserver::~TimerObserver()
{
}

void TimerObserver::on_signal_1_second()
{

}

void TimerObserver::on_signal_10_seconds()
{

}
//LCOV_EXCL_STOP

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::attach(TimerObserver *observer)
{
	observers.push_back(observer);
}

void Timer::detach(TimerObserver *observer)
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
	static int TenSecondCounter = 10;
	TenSecondCounter--;
	bool signal10Seconds = false;
	if (TenSecondCounter < 1)
	{
		signal10Seconds = true;
		TenSecondCounter = 10;
	}
	if (observers.size() > 0)
	{
		for (TimerObserver *observer : observers)
		{
			observer->on_signal_1_second();
			if (signal10Seconds)
			{
				observer->on_signal_10_seconds();
			}
		}
	}

}

}
