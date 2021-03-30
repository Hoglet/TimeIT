/*
 * Notifier.h
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */
#ifndef TIMEIT_NOTIFIER_H
#define TIMEIT_NOTIFIER_H

#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <libtimeit/Event_observer.h>
#include <libtimeit/db/message_type.h>

namespace libtimeit
{
using namespace std;

struct Notification_message
{
	message_type type;
	int64_t      task_ID;
};

class Notifier
{
public:
	Notifier();
	virtual ~Notifier();
	void attach(Event_observer*);
	void detach(Event_observer*);
	void send_notification(message_type type, int64_t taskId);
	void enabled(bool);
	void send(EventType type, const string Headline, const string message);
	int size();
private:
	void send_message(Notification_message);

	list<Event_observer*> observers = {};
	bool enabled_ = true;
	bool missed_notification = false;
};

}
#endif