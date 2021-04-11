#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "task.h"

namespace libtimeit
{
class Extended_task : public Task
{
	friend class Extended_task_accessor;
public:
	const bool expanded_;
	const bool running_;
	const int  time_;

	int total_time() const;
	Extended_task(
			int           ID,
			int           op_parent_ID,
			const string& op_name,
			int           op_time = 0,
			bool          op_expanded = false,
			bool          op_running = false,
			int           op_total_time = 0);
private:
	int  total_time_;
};
}

#endif