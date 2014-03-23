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

namespace DB
{

class Notifier
{
public:
	Notifier();
	virtual ~Notifier();
	void attach(TaskAccessorObserver*);
	void detach(TaskAccessorObserver*);
	void taskUpdated(int64_t taskID);
	void taskRemoved(int64_t taskID);
	void taskAdded(int64_t taskID);
	void taskParentChanged(int64_t taskID);
	void enabled(bool);
private:
	std::list<TaskAccessorObserver*> observers;
	bool m_enabled;
	bool m_missedNotification;
};

}
#endif /* NOTIFIER_H_ */
