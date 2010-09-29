/*
 * ITimeKeeper.h
 *
 *  Created on: Jan 18, 2009
 *      Author: hoglet
 */

#ifndef ITIMEKEEPER_H_
#define ITIMEKEEPER_H_

#include <IGUIFactory.h>

class TimekeeperObserver
{
public:
	virtual void on_idleDetected() = 0;
	virtual void on_runningChanged() = 0;
};

class ITimeKeeper
{
public:
	virtual void StartTask(int64_t id) = 0;
	virtual void StopTask(int64_t id) = 0;
	virtual void ToggleTask(int64_t id) = 0;

	virtual bool hasRunningTasks() = 0;
	virtual time_t timeIdle() = 0;

	//Enable (or disable) automatic time keeping.
	virtual void enable(bool) = 0;
	//Stop all tasks without saving new time records
	virtual void stopAll() = 0;
	//Stop all tasks, without saving new time records, and then start them again
	virtual void stopAllAndContinue() = 0;

	//
	virtual void attach(TimekeeperObserver*) = 0;
	virtual void detach(TimekeeperObserver*) = 0;
};

#endif /* ITIMEKEEPER_H_ */
