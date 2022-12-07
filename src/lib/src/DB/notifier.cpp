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



void notification_manager::attach(event_observer* observer)
{
	if (observer != nullptr)
	{
		observers.push_back(observer);
	}
}
void notification_manager::detach(event_observer* observer)
{
	if (observer != nullptr)
	{
		observers.remove(observer);
	}
}

void notification_manager::send( event_type type, string headline, string message)
{
	for (event_observer *observer : observers)
	{
		observer->on_message(type, headline, message);
	}
}


void notification_manager::send_message(notification_message message)
{
	switch(message.type)
	{
		case complete_update:
			this->broadcast(
					[message](event_observer* observer)
					{
						observer->on_complete_update();
					}
			);
			break;
		case settings_changed:
			this->broadcast(
					[message](event_observer* observer)
					{
						observer->on_settings_changed( message.name );
					}
			);
			break;
		case show_main_window:
			this->broadcast(
					[message](event_observer* observer)
					{
						observer->on_show_main_window();
					}
			);
			break;
		default:
			throw ("Unknown message type");
	}
}

void notification_manager::is_enabled(bool op_enabled)
{
	if ( op_enabled != enabled)
	{
		enabled = op_enabled;
	}
	if (enabled && missed_notification)
	{
		missed_notification = false;
		try_send_notification( complete_update, 0);
	}
}

void notification_manager::send_notification(message_type type, int64_t item_id, string name)
{
	notification_message message{type, item_id, name};
	send_message(message);
}

void notification_manager::try_send_notification(message_type type, int64_t item_id, string name)
{
	if (enabled)
	{
		send_notification( type, item_id, name);
	}
	else
	{
		missed_notification = true;
	}
}

unsigned long notification_manager::size()
{
	return observers.size();
}

void notification_manager::broadcast(function<void(event_observer*)> function)
{
	for (event_observer *observer : observers)
	{
		function(observer);
	}
}

}
