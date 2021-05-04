#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "task.h"

namespace libtimeit
{
struct Extended_task : public Task
{
	friend class Extended_task_accessor;

	const bool expanded;
	const bool running;
	const int  time;
	const int  total_time;

	Extended_task(
			int           ID,
			int           parent_ID,
			const string& name,
			int           time = 0,
			bool          expanded = false,
			bool          running = false,
			int           total_time = 0);

//	Extended_task with_total(int i) const;
};
}

#endif