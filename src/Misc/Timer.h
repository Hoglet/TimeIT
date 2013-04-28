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
}

class TimerObserver
{
public:
	virtual ~TimerObserver()
	{
	}
	virtual void on_signal_1_second() {};
	virtual void on_signal_10_seconds() {};
};

class Timer
{
	friend class Test::TimerTest;
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
