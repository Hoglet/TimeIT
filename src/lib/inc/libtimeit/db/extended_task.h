#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "task.h"

namespace libtimeit
{
class Extended_task : public Task
{
	friend class Extended_task_accessor;
public:
	const bool expanded;
	const bool running;
	const int  time;

	int total_time() const;
	Extended_task(
			int           ID,
			int           parent_ID,
			const string& name,
			int           time = 0,
			bool          expanded = false,
			bool          running = false,
			int           total_time = 0);
private:
	int  total_time_;
};
}

#endif