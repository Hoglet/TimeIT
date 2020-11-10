#pragma once
#include "Task.h"

namespace DB
{
class ExtendedTask : public Task
{
	friend class ExtendedTaskAccessor;
public:
	ExtendedTask(
			int                ID,
			int                parentID,
			const std::string& name,
			int                time = 0,
			bool               expanded = false,
			bool               running = false,
			int                totalTime = 0);

	bool expanded()  const;
	int  totalTime() const;
	int  time()      const;
	bool running()   const;

protected:
	bool expanded_;
	bool running_;
	int totalTime_;
	int time_;
private:
	ExtendedTask();
	void operator=(const ExtendedTask&);
};
}

