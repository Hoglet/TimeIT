/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "Notifier.h"

Notifier::Notifier()
{
}

Notifier::~Notifier()
{
}

void Notifier::taskUpdated(int64_t taskID)
{
	for (TaskAccessorObserver* observer : observers)
	{
		observer->on_taskUpdated(taskID);
	}
}

void Notifier::taskAdded(int64_t taskID)
{
	for (TaskAccessorObserver* observer : observers)
	{
		observer->on_taskAdded(taskID);
	}
}

void Notifier::taskParentChanged(int64_t taskID)
{
	for (TaskAccessorObserver* observer : observers)
	{
		observer->on_taskParentChanged(taskID);
	}
}

void Notifier::taskRemoved(int64_t taskID)
{
	for (TaskAccessorObserver* observer : observers)
	{
		observer->on_taskRemoved(taskID);
	}
}

void Notifier::attach(TaskAccessorObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}

void Notifier::detach(TaskAccessorObserver* observer)
{
	if (observer)
	{
		observers.remove(observer);
	}
}
