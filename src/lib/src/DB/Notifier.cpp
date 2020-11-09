/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "Notifier.h"
#include "Utils.h"
#include <iostream>

namespace DB
{
	using namespace std;


    TaskAccessorObserver::~TaskAccessorObserver()
	{

	}

	void TaskAccessorObserver::on_taskParentChanged(int64_t)
	{
	}

	Notifier::Notifier()
	{
		m_enabled = true;
		m_missedNotification = false;
	}

	Notifier::~Notifier()
	{

	}

	void Notifier::sendMessage(NotificationMessage message)
	{
		if (message.type == TASK_UPDATED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskUpdated(message.taskID);
			}
		}
		else if (message.type == TASK_NAME_CHANGED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskNameChanged(message.taskID);
			}
		}
		else if (message.type == TASK_TIME_CHANGED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskTimeChanged(message.taskID);
			}
		}
		else if (message.type == TASK_ADDED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskAdded(message.taskID);
			}
		}
		else if (message.type == TASK_REMOVED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskRemoved(message.taskID);
			}
		}
		else if (message.type == TASK_PARENT_CHANGED)
		{
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_taskParentChanged(message.taskID);
			}
		}
		else if (message.type == COMPLETE_UPDATE)
		{
			uint32_t start = Utils::millisecondsSinceEpoch();
			for (TaskAccessorObserver *observer : observers)
			{
				observer->on_completeUpdate();
			}
			uint32_t stop = Utils::millisecondsSinceEpoch();
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

	void Notifier::attach(TaskAccessorObserver *observer)
	{
		if (observer)
		{
			observers.push_back(observer);
		}
	}

	void Notifier::detach(TaskAccessorObserver *observer)
	{
		if (observer)
		{
			observers.remove(observer);
		}
	}
}
