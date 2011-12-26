/*
 * Notifier.h
 *
 *  Created on: Dec 25, 2011
 *      Author: hoglet
 */

#ifndef NOTIFIER_H_
#define NOTIFIER_H_
#include <stdint.h>
#include <ITaskAccessor.h>
#include <list>


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
private:
	std::list<TaskAccessorObserver*> observers;

};

#endif /* NOTIFIER_H_ */
