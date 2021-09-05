#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

extended_task::extended_task(
		const task&       source,
		seconds           op_time,
		seconds           op_total_time,
		bool              op_expanded,
		bool              op_running
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
		expanded(op_expanded),
		running(op_running),
		time(op_time),
		total_time( op_total_time)
{

}



}
