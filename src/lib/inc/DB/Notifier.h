/*
 * Notifier.h
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#pragma once

#include "TaskAccessorObserver.h"
#include <cstdint>
#include <list>

namespace DB
{
enum MessageType
{
	TASK_UPDATED,
	TASK_REMOVED,
	TASK_ADDED,
	TASK_PARENT_CHANGED,
	COMPLETE_UPDATE,
	TASK_NAME_CHANGED,
	TASK_TIME_CHANGED
};
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
	void attach(TaskAccessorObserver*);
	void detach(TaskAccessorObserver*);
	void sendNotification(MessageType type, int64_t taskId);
	void enabled(bool);
private:
	void sendMessage(NotificationMessage);
	std::list<TaskAccessorObserver*> observers;

	bool m_enabled;
	bool m_missedNotification;
};

}
