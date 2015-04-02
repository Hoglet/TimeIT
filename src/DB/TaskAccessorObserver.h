#ifndef TASKACCESSOR_OBSERVER_H_
#define TASKACCESSOR_OBSERVER_H_

#include <vector>
#include <stdint.h>
#include <memory>
#include "Task.h"

namespace DB
{
class TaskAccessorObserver
{
public:
	virtual ~TaskAccessorObserver();
	virtual void on_taskAdded(int64_t)  = 0;
	virtual void on_taskUpdated(int64_t) = 0;
	virtual void on_taskRemoved(int64_t) = 0;
	virtual void on_taskParentChanged(int64_t);
	virtual void on_completeUpdate() = 0;
};
}

#endif /* TASKACCESSOR_OBSERVER_H_ */
