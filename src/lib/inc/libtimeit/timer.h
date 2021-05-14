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
	Timer_observer(const Timer_observer&) = delete;
	Timer_observer(const Timer_observer&&) = delete;
	Timer_observer& operator=(const Timer_observer&) = delete;
	Timer_observer& operator=(const Timer_observer&&) = delete;
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
	// Call this every second
	bool on_signal_1_second();
private:
	void attach(Timer_observer *observer);
	void detach(Timer_observer *observer);

	void signal_sender();
	list<Timer_observer*> observers = {};
	int ten_second_counter{10};  // NOLINT
};
}
#endif