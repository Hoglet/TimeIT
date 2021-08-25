#ifndef EXTENDED_TASK_H
#define EXTENDED_TASK_H

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
			long              op_total_time,
			bool              expanded,
			bool              running
			);
};
}

#endif