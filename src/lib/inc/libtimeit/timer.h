/*
 * Timer.h
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */
#ifndef TIMEIT_TIMER_H
#define TIMEIT_TIMER_H

#include <list>

namespace libtimeit
{

class Timer;

using namespace std;

class Timer_observer
{
public:
	Timer_observer(Timer& publisher);
	virtual ~Timer_observer();
	//LCOV_EXCL_START
	virtual void on_signal_1_second();
	virtual void on_signal_10_seconds();
	//LCOV_EXCL_STOP
private:
	Timer& timer;
};

class Timer
{
	friend class Timer_observer;

public:
	Timer();
	virtual ~Timer();

	// Call this every second glibmm example:
	// Glib::signal_timeout().connect_seconds(sigc::mem_fun(timer, &Timer::on_signal_1_second), 1);
	bool on_signal_1_second();
private:
	void attach(Timer_observer *observer);
	void detach(Timer_observer *observer);

	void signalSender();
	list<Timer_observer *> observers = {};
};
}
#endif