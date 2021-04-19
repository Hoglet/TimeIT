#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

Extended_task::Extended_task(
		int           ID,
		int           parent_ID,
		const string& name,
		int           time,
		bool          expanded,
		bool          running,
		int           op_total_time)
		:
		Task(
				name,
				parent_ID,
				UUID(),
				false,
				ID,
				0,
				{},
				false,
				0,
				false
				),
		total_time_(op_total_time),
		expanded(expanded),
		running(running),
		time(time)
{
}

int Extended_task::total_time() const
{
	return total_time_;
}

}
