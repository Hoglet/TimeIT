#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "libtimeit/datatypes/task.h"

namespace libtimeit
{
struct extended_task : public task
{
	friend class extended_task_accessor;

	const bool expanded;
	const bool running;
	const long time;
	const long total_time;

	extended_task(
			const task&       source,
			long              time,
			long              total_time,
			bool              expanded,
			bool              running
			);



	extended_task(
			task_id           uuid,
			optional<task_id> parent_ID,
			const string&     name,
			long              time = 0,
			bool              expanded = false,
			bool              running = false,
			long              total_time = 0
	);

//	Extended_task with_total(int i) const;
};
}

#endif