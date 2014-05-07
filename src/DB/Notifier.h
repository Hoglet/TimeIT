/*
 * Notifier.h
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#ifndef NOTIFIER_H_
#define NOTIFIER_H_

#include "ITaskAccessor.h"
#include <stdint.h>
#include <list>
#include <deque>
#include <glibmm.h>


namespace DB
{
enum MessageType
{
	TASK_UPDATED,
	TASK_REMOVED,
	TASK_ADDED,
	TASK_PARENT_CHANGED,
	COMPLETE_UPDATE
};
struct NotificationMessage
{
	MessageType type;
	int64_t	taskID;
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
	void messageForwarder();
	std::list<TaskAccessorObserver*> observers;
	std::deque<NotificationMessage> messageQue;
	Glib::Dispatcher signal_message;
	bool m_enabled;
	bool m_missedNotification;
	Glib::Mutex mutex;
	Glib::Thread*  receiving_thread;
};

}
#endif /* NOTIFIER_H_ */
