#ifndef TIMEIT_EXTENDED_TASK_H
#define TIMEIT_EXTENDED_TASK_H

#include "task.h"

namespace libtimeit
{
class Extended_task : public Task
{
	friend class Extended_task_accessor;
public:
	Extended_task(
			int           ID,
			int           parent_ID,
			const string& name,
			int           time = 0,
			bool          expanded = false,
			bool          running = false,
			int           total_time = 0);

	bool expanded()   const;
	int  total_time() const;
	int  time()       const;
	bool running()    const;

protected:
	bool expanded_;
	bool running_;
	int  total_time_;
	int  time_;
private:
	Extended_task();
	void operator=(const Extended_task&);
};
}

#endif