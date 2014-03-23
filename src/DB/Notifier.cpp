/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "Notifier.h"

namespace DB
{

Notifier::Notifier()
{
	 m_enabled=true;
	 m_missedNotification=false;
}

Notifier::~Notifier()
{
}

void Notifier::enabled(bool state)
{
	if (m_enabled != state)
	{
		if (state == true && m_missedNotification)
		{
			m_missedNotification = false;
			for (TaskAccessorObserver* observer : observers)
			{
				observer->on_completeUpdate();
			}
		}
		m_enabled = state;
	}
}

void Notifier::taskUpdated(int64_t taskID)
{
	if (m_enabled)
	{
		for (TaskAccessorObserver* observer : observers)
		{
			observer->on_taskUpdated(taskID);
		}
	} else
	{
		m_missedNotification = true;
	}
}

void Notifier::taskAdded(int64_t taskID)
{
	if (m_enabled)
	{
		for (TaskAccessorObserver* observer : observers)
		{
			observer->on_taskAdded(taskID);
		}
	} else
	{
		m_missedNotification = true;
	}
}

void Notifier::taskParentChanged(int64_t taskID)
{
	if (m_enabled)
	{

		for (TaskAccessorObserver* observer : observers)
		{
			observer->on_taskParentChanged(taskID);
		}
	} else
	{
		m_missedNotification = true;
	}
}

void Notifier::taskRemoved(int64_t taskID)
{
	if (m_enabled)
	{

		for (TaskAccessorObserver* observer : observers)
		{
			observer->on_taskRemoved(taskID);
		}
	} else
	{
		m_missedNotification = true;
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
}
