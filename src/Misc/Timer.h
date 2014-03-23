/*
 * Timer.h
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <list>

namespace Test
{
	class TimerTest;
	class DummyTimerTest;
}

class TimerObserver
{
public:
	virtual ~TimerObserver();
	//LCOV_EXCL_START
	virtual void on_signal_1_second();
	virtual void on_signal_10_seconds();
	//LCOV_EXCL_STOP
};

class Timer
{
	friend class Test::TimerTest;
	friend class Test::DummyTimerTest;
public:
	Timer();
	virtual ~Timer();
	void attach(TimerObserver* observer);
	void detach(TimerObserver* observer);
protected:
	bool on_signal_1_second();
private:
	void signalSender();
	std::list<TimerObserver*> observers;
};

#endif /* TIMER_H_ */
