#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

extended_task::extended_task(
		int           ID,
		UUID          uuid,
		int           parent_ID,
		const string& name,
		int           time,
		bool          expanded,
		bool          running,
		int           total_time_)
		:
		task(
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
		expanded(expanded),
		running(running),
		time(time),
		total_time(total_time_)
{
}

}
