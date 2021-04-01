/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "libtimeit/db/notifier.h"
#include "libtimeit/Utils.h"
#include <iostream>

namespace libtimeit
{
using namespace std;



Notifier::Notifier()
{
}

Notifier::~Notifier()
{

}

void Notifier::attach(Event_observer* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}
void Notifier::detach(Event_observer* observer)
{
	if (observer)
	{
		observers.remove(observer);
	}
}

void Notifier::send(EventType type, const string headline, const string message)
{
	for (Event_observer *observer : observers)
	{
		observer->on_message(type, headline, message);
	}
}


void Notifier::send_message(Notification_message message)
{
	switch(message.type)
	{
		case TASK_UPDATED:
			for (Event_observer *observer : observers)
			{
				observer->on_task_updated(message.task_ID);
			}
			break;
		case  TASK_NAME_CHANGED:
			for (Event_observer *observer : observers)
			{
				observer->on_task_name_changed(message.task_ID);
			}
			break;
		case TASK_TIME_CHANGED:
			for (Event_observer *observer : observers)
			{
				observer->on_task_time_changed(message.task_ID);
			}
			break;
		case TASK_ADDED:
			for (Event_observer *observer : observers)
			{
				observer->on_task_added(message.task_ID);
			}
			break;
		case TASK_REMOVED:
			for (Event_observer *observer : observers)
			{
				observer->on_task_removed(message.task_ID);
			}
			break;
		case TASK_PARENT_CHANGED:
			for (Event_observer *observer : observers)
			{
				observer->on_parent_changed(message.task_ID);
			}
		case COMPLETE_UPDATE:
			for (Event_observer *observer : observers)
			{
				observer->on_complete_update();
			}
			break;
		case SETTINGS_CHANGED:
			for (Event_observer *observer : observers)
			{
				observer->on_settings_changed( message.name );
			}
			break;
		default:
			throw ("Unknown message type");
	}
}

void Notifier::enabled(bool enabled)
{
	if (enabled_ != enabled)
	{
		enabled_ = enabled;
	}
	if (enabled && missed_notification)
	{
		missed_notification = false;
		send_notification(COMPLETE_UPDATE, 0);
	}
}


void Notifier::send_notification(message_type type, int64_t taskId, string name)
{
	Notification_message message{type, taskId, name};

	if (enabled_)
	{
		send_message(message);
	}
	else
	{
		missed_notification = true;
	}
}

int Notifier::size()
{
	return observers.size();
}
}
