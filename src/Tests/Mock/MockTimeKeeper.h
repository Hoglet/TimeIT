/*
 * MockTimeKeeper.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKTIMEKEEPER_H_
#define MOCKTIMEKEEPER_H_


#include "ITimeKeeper.h"
#include <stdint.h>

namespace Test
{

class MockTimeKeeper: public ITimeKeeper
{
public:
	MockTimeKeeper();
	virtual ~MockTimeKeeper();

	virtual void StartTask(int64_t id) ;
	virtual void StopTask(int64_t id) ;
	virtual void ToggleTask(int64_t id);

	virtual bool hasRunningTasks() ;
	virtual time_t timeIdle();

	//Enable (or disable) automatic time keeping.
	virtual void enable(bool);
	//Stop all tasks without saving new time records
	virtual void stopAll() ;
	//Stop all tasks, without saving new time records, and then start them again
	virtual void stopAllAndContinue();

	//
	virtual void attach(TimekeeperObserver*);
	virtual void detach(TimekeeperObserver*);
	int64_t startedTask;
	int64_t stopedTask;
};

}
#endif /* MOCKTIMEKEEPER_H_ */
