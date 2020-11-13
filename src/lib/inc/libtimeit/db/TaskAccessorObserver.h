#pragma once

#include <cstdint>

namespace DB
{
class TaskAccessorObserver
{
public:
	virtual      ~TaskAccessorObserver();
	virtual void on_taskAdded(int64_t)          = 0;
	virtual void on_taskUpdated(int64_t)        = 0;
	virtual void on_taskRemoved(int64_t)        = 0;
	virtual void on_taskParentChanged(int64_t);
	virtual void on_completeUpdate()            = 0;
	virtual void on_taskNameChanged(int64_t)    = 0;
	virtual void on_taskTimeChanged(int64_t)    = 0;
};
}

