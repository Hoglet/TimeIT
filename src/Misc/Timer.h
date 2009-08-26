/*
 * Timer.h
 *
 *  Created on: 2008-jul-04
 *      Author: hoglet
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <list>

class TimerObserver
{
public:
	virtual void on_signal_1_second() = 0;
	virtual void on_signal_10_seconds() = 0;
};

class Timer
{
public:
	Timer();
	virtual ~Timer();
	void attach(TimerObserver* observer);
	void detach(TimerObserver* observer);
private:
	bool on_signal_1_second();
	void signalSender();
	std::list<TimerObserver*> observers;
};

#endif /* TIMER_H_ */
