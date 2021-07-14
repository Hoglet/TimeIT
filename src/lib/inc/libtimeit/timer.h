#ifndef TIMEIT_TIMER_H
#define TIMEIT_TIMER_H

#include <list>

namespace libtimeit
{

class Timer;

using namespace std;

class timer_observer
{
public:
	timer_observer(Timer& publisher);
	timer_observer(const timer_observer&) = delete;
	timer_observer(const timer_observer&&) = delete;
	timer_observer& operator=(const timer_observer&) = delete;
	timer_observer& operator=(const timer_observer&&) = delete;
	virtual ~timer_observer();

	//LCOV_EXCL_START
	virtual void on_signal_1_second();
	virtual void on_signal_10_seconds();
	//LCOV_EXCL_STOP
private:
	Timer& timer;
};

class Timer
{
	friend class timer_observer;
public:
	// Call this every second
	bool on_signal_1_second();
private:
	void attach(timer_observer *observer);
	void detach(timer_observer *observer);

	void signal_sender();
	list<timer_observer*> observers = {};
	int ten_second_counter{10};  // NOLINT
};
}
#endif