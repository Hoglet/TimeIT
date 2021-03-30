#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

bool Extended_task::expanded() const
{
	return expanded_;
}

bool Extended_task::running() const
{
	return running_;
}

Extended_task::Extended_task(
		int           ID,
		int           parent_ID,
		const string& name,
		int           time,
		bool          expanded,
		bool          running,
		int           total_time)
		:
		Task(name, parent_ID),
		total_time_(total_time),
		expanded_(expanded),
		running_(running),
		time_(time)
{
	ID_ = ID;
}

int Extended_task::total_time() const
{
	return total_time_;
}

int Extended_task::time() const
{
	return time_;
}

}
