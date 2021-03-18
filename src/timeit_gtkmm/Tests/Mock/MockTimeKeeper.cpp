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
	startedTask = 0;
	stopedTask = 0;
}

MockTimeKeeper::~MockTimeKeeper()
{
}

void MockTimeKeeper::StartTask(int64_t id)
{
	startedTask=id;
}

void MockTimeKeeper::StopTask(int64_t id)
{
	stopedTask=id;
}
void MockTimeKeeper::ToggleTask(int64_t id)
{
	;
}

bool MockTimeKeeper::isActiveTask(int64_t taskID)
{
	return false;
}
bool MockTimeKeeper::hasRunningTasks()
{
	return false;
}

bool MockTimeKeeper::isIdle()
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
