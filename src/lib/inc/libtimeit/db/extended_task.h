#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "task.h"

namespace libtimeit
{
struct extended_task : public task
{
	friend class extended_task_accessor;

	const bool expanded;
	const bool running;
	const int  time;
	const int  total_time;

	extended_task(
			int           ID,
			UUID          uuid,
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