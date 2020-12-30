/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "libtimeit/db/Notifier.h"
#include "libtimeit/Utils.h"
#include <iostream>

namespace libtimeit
{
using namespace std;



Notifier::Notifier()
	{
		m_enabled = true;
		m_missedNotification = false;
	}

	Notifier::~Notifier()
	{

	}

void Notifier::send(EventType type, const std::string headline, const std::string message)
{
	for (EventObserver *observer : observers)
	{
		observer->on_message(type, headline, message);
	}
}


void Notifier::sendMessage(NotificationMessage message)
	{
		if (message.type == TASK_UPDATED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskUpdated(message.taskID);
			}
		}
		else if (message.type == TASK_NAME_CHANGED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskNameChanged(message.taskID);
			}
		}
		else if (message.type == TASK_TIME_CHANGED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskTimeChanged(message.taskID);
			}
		}
		else if (message.type == TASK_ADDED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskAdded(message.taskID);
			}
		}
		else if (message.type == TASK_REMOVED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskRemoved(message.taskID);
			}
		}
		else if (message.type == TASK_PARENT_CHANGED)
		{
			for (EventObserver *observer : observers)
			{
				observer->on_taskParentChanged(message.taskID);
			}
		}
		else if (message.type == COMPLETE_UPDATE)
		{
			uint32_t start = millisecondsSinceEpoch();
			for (EventObserver *observer : observers)
			{
				observer->on_completeUpdate();
			}
			uint32_t stop = millisecondsSinceEpoch();
			std::cout << "Complete update of GUI: " << stop - start << " ms.\n";
		}
		else
		{
			throw ("Unknown message type");
		}
	}

	void Notifier::enabled(bool enabled)
	{
		if (m_enabled != enabled)
		{
			m_enabled = enabled;
		}
		if (enabled && m_missedNotification)
		{
			m_missedNotification = false;
			sendNotification(COMPLETE_UPDATE, 0);
		}
	}

	void Notifier::sendNotification(MessageType type, int64_t taskID)
	{
		NotificationMessage message{type,taskID};

		if (m_enabled)
		{
			sendMessage(message);
		}
		else
		{
			m_missedNotification = true;
		}
	}

	void Notifier::attach(EventObserver *observer)
	{
		if (observer)
		{
			observers.push_back(observer);
		}
	}

	void Notifier::detach(EventObserver *observer)
	{
		if (observer)
		{
			observers.remove(observer);
		}
	}

int Notifier::size()
{
	return observers.size();
}
}
