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
#include <libtimeit/EventObserver.h>
#include <libtimeit/db/MessageType.h>

namespace libtimeit
{


struct NotificationMessage
{
	MessageType type;
	int64_t taskID;
};

class Notifier
{
public:
	Notifier();
	virtual ~Notifier();
	void attach(EventObserver*);
	void detach(EventObserver*);
	void sendNotification(MessageType type, int64_t taskId);
	void enabled(bool);
	void send(EventType type, const std::string Headline, const std::string message);
	int size();
private:
	void sendMessage(NotificationMessage);
	std::list<EventObserver*> observers = {};

	bool m_enabled;
	bool m_missedNotification;
};

}
#endif