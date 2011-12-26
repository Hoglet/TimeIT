/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "Notifier.h"



Notifier::Notifier()
{
	// TODO Auto-generated constructor stub

}

Notifier::~Notifier()
{
	// TODO Auto-generated destructor stub
}

void Notifier::taskUpdated(int64_t taskID)
{
	try
	{
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskUpdated(taskID);
		}
	} catch (...)
	{
	}
}
void Notifier::taskAdded(int64_t taskID)
{
	try
	{
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskAdded(taskID);
		}
	} catch (...)
	{
	}
}
void Notifier::taskParentChanged(int64_t taskID)
{
	try
	{
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskParentChanged(taskID);
		}
	} catch (...)
	{
	}
}

void Notifier::taskRemoved(int64_t taskID)
{
	try
	{
		std::list<TaskAccessorObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); iter++)
		{
			TaskAccessorObserver* observer = *iter;
			observer->on_taskRemoved(taskID);
		}
	} catch (...)
	{
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




