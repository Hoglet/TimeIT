#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

extended_task::extended_task(
		const task&       source,
		long              time,
		long              total_time_,
		bool              expanded,
		bool              running
		)
		:
		task(
				source.name,
				source.id,
				source.last_changed,
				source.parent_id,
				source.deleted,
				source.idle,
				source.quiet
		),
		expanded(expanded),
		running(running),
		time(time),
		total_time(total_time_)
{

}



}
