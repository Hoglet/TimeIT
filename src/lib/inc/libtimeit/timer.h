/*
 * Timer.h
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */
#ifndef TIMEIT_TIMER_H
#define TIMEIT_TIMER_H

#include <list>

namespace Test
{
class TimerTest;
class DummyTimerTest;
}
namespace libtimeit
{

class Timer;

using namespace std;

class TimerObserver
{
public:
	TimerObserver(Timer& publisher);
	virtual ~TimerObserver();
	//LCOV_EXCL_START
	virtual void on_signal_1_second();
	virtual void on_signal_10_seconds();
	//LCOV_EXCL_STOP
private:
	Timer& timer;
};

class Timer
{
	friend class Test::TimerTest;
	friend class TimerObserver;
	friend class Test::DummyTimerTest;

public:
	Timer();
	virtual ~Timer();

	// Call this every second glibmm example:
	// Glib::signal_timeout().connect_seconds(sigc::mem_fun(timer, &Timer::on_signal_1_second), 1);
	bool on_signal_1_second();
private:
	void attach(TimerObserver *observer);
	void detach(TimerObserver *observer);

	void signalSender();
	list<TimerObserver *> observers = {};
};
}
#endif