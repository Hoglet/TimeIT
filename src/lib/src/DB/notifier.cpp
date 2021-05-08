/*
 * Notifier.cpp
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#include "libtimeit/db/notifier.h"

namespace libtimeit
{
using namespace std;



void Notifier::attach(Event_observer* observer)
{
	if (observer != nullptr)
	{
		observers.push_back(observer);
	}
}
void Notifier::detach(Event_observer* observer)
{
	if (observer != nullptr)
	{
		observers.remove(observer);
	}
}

void Notifier::send(EventType type, string headline, string message)
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
			this->notify_all( [message](Event_observer* observer)
							  {
								  observer->on_task_updated(message.ID);
							  }
							  );
			break;
		case  TASK_NAME_CHANGED:
			this->notify_all( [message](Event_observer* observer)
								{
									observer->on_task_name_changed(message.ID);
								}
							);
			break;
		case TASK_TIME_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_time_changed(message.ID);
					}
					);
			break;
		case TASK_ADDED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_added(message.ID);
					}
					);
			break;
		case TASK_REMOVED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_removed(message.ID);
					}
			);
			break;
		case TASK_PARENT_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_parent_changed(message.ID);
					}
			);
		case COMPLETE_UPDATE:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_complete_update();
					}
			);
			break;
		case SETTINGS_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_settings_changed( message.name );
					}
			);
			break;
		case TIME_ENTRY_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_time_entry_changed( message.ID );
					}
			);
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


void Notifier::send_notification(message_type type, int64_t ID, string name)
{
	Notification_message message{type, ID, name};

	if (enabled_)
	{
		send_message(message);
	}
	else
	{
		missed_notification = true;
	}
}

unsigned long Notifier::size()
{
	return observers.size();
}

void Notifier::notify_all(function<void(Event_observer*)> function)
{
	for (Event_observer *observer : observers)
	{
		function(observer);
	}
}

}