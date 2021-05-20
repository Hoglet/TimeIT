/*
 * Notifier.h
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */
#ifndef TIMEIT_NOTIFIER_H
#define TIMEIT_NOTIFIER_H

#include <cstdint>
#include <list>
#include <string>
#include <functional>
#include <libtimeit/event_observer.h>
#include <libtimeit/db/message_type.h>

namespace libtimeit
{
using namespace std;

struct Notification_message
{
	message_type type;
	int64_t      id;
	string       name;
};

class Notifier
{
	friend class Event_observer;
public:
	void             try_send_notification(message_type type, int64_t item_id, string name = "");
	void             send_notification(message_type type, int64_t item_id, string name = "");
	void             is_enabled(bool enabled_);
	void             send(EventType type, string headline, string message);
	unsigned long    size();
protected:
	void attach(Event_observer*);
	void detach(Event_observer*);
private:
	void send_message(Notification_message);

	bool enabled = true;
	bool missed_notification = false;

	list<Event_observer*> observers = {};

	void notify_all(function<void(Event_observer*)>);
};

}
#endif