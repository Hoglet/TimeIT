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
	m_enabled = true;
	m_missedNotification = false;
	receiving_thread = Glib::Thread::self();
	signal_message.connect(sigc::mem_fun(*this, &Notifier::messageForwarder));
}

Notifier::~Notifier()
{
}

void Notifier::messageForwarder()
{
	if (m_enabled)
	{
		while (!messageQue.empty())
		{
			Glib::Mutex::Lock lock(mutex);
			NotificationMessage message = messageQue.back();
			messageQue.pop_back();
			lock.release();
			if (message.type == TASK_UPDATED)
			{
				for (TaskAccessorObserver* observer : observers)
				{
					observer->on_taskUpdated(message.taskID);
				}
			}
			else if (message.type == TASK_ADDED)
			{
				for (TaskAccessorObserver* observer : observers)
				{
					observer->on_taskAdded(message.taskID);
				}
			}
			else if (message.type == TASK_REMOVED)
			{
				for (TaskAccessorObserver* observer : observers)
				{
					observer->on_taskRemoved(message.taskID);
				}
			}
			else if (message.type == TASK_PARENT_CHANGED)
			{
				for (TaskAccessorObserver* observer : observers)
				{
					observer->on_taskParentChanged(message.taskID);
				}
			}
			else if (message.type == COMPLETE_UPDATE)
			{
				for (TaskAccessorObserver* observer : observers)
				{
					observer->on_completeUpdate();
				}
			}
			else
			{
				throw("Unknown message type");
			}
		}
	}
	else
	{
		m_missedNotification = true;
	}
}

void Notifier::enabled(bool state)
{
	Glib::Mutex::Lock lock(mutex);
	if (m_enabled != state)
	{
		m_enabled = state;
		lock.release();
		if (state == true && m_missedNotification)
		{
			m_missedNotification = false;
			sendNotification(COMPLETE_UPDATE, 0);
		}
	}
}

void Notifier::sendNotification(MessageType type, int64_t taskID)
{
	NotificationMessage message;
	message.type = type;
	message.taskID = taskID;
	Glib::Mutex::Lock lock(mutex);
	messageQue.push_front(message);
	lock.release();

	if (Glib::Thread::self() == receiving_thread )
	{
		messageForwarder();
	}
	else
	{
		signal_message();
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
