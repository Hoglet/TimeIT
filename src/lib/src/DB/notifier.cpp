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
								  observer->on_task_updated(message.id);
							  }
							  );
			break;
		case  TASK_NAME_CHANGED:
			this->notify_all( [message](Event_observer* observer)
								{
									observer->on_task_name_changed(message.id);
								}
							);
			break;
		case TASK_TIME_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_time_changed(message.id);
					}
					);
			break;
		case TASK_ADDED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_added(message.id);
					}
					);
			break;
		case TASK_REMOVED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_task_removed(message.id);
					}
			);
			break;
		case TASK_PARENT_CHANGED:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_parent_changed(message.id);
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
						observer->on_time_entry_changed( message.id );
					}
			);
			break;
		case SHOW_MAIN_WINDOW:
			this->notify_all(
					[message](Event_observer* observer)
					{
						observer->on_show_main_window();
					}
			);
			break;
		default:
			throw ("Unknown message type");
	}
}

void Notifier::is_enabled(bool enabled_)
{
	if (enabled_ != enabled)
	{
		enabled = enabled_;
	}
	if (enabled && missed_notification)
	{
		missed_notification = false;
		try_send_notification(COMPLETE_UPDATE, 0);
	}
}

void Notifier::send_notification(message_type type, int64_t item_id, string name)
{
	Notification_message message{type, item_id, name};
	send_message(message);
}

void Notifier::try_send_notification(message_type type, int64_t item_id, string name)
{
	if (enabled)
	{
		send_notification( type, item_id, name);
	}
	missed_notification = true;
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
