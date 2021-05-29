#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

Extended_task::Extended_task(
		int           ID,
		UUID          uuid,
		int           parent_ID,
		const string& name,
		int           time,
		bool          expanded,
		bool          running,
		int           total_time_)
		:
		Task(
				name,
				parent_ID,
				uuid,
				false,
				ID,
				0,
				{},
				false,
				0,
				false
				),
		total_time(total_time_),
		expanded(expanded),
		running(running),
		time(time)
{
}

}
