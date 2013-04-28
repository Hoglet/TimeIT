/*
 * MockTimeKeeper.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockTimeKeeper.h"

namespace Test
{

MockTimeKeeper::MockTimeKeeper()
{
}

MockTimeKeeper::~MockTimeKeeper()
{
}

void MockTimeKeeper::StartTask(int64_t id)
{
	;
}

void MockTimeKeeper::StopTask(int64_t id)
{
	;
}
void MockTimeKeeper::ToggleTask(int64_t id)
{
	;
}

bool MockTimeKeeper::hasRunningTasks()
{
	return false;
}

time_t MockTimeKeeper::timeIdle()
{
	return 0;
}

void MockTimeKeeper::enable(bool)
{
	;
}

void MockTimeKeeper::stopAll()
{
	;
}

void MockTimeKeeper::stopAllAndContinue()
{
	;
}

void MockTimeKeeper::attach(TimekeeperObserver*)
{
	;
}

void MockTimeKeeper::detach(TimekeeperObserver*)
{
	;
}



}
