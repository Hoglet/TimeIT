#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

extended_task::extended_task(
		task_id           uuid,
		optional<task_id> parent_ID,
		const string&     name,
		long              time,
		bool              expanded,
		bool              running,
		long              total_time_)
		:
		task(
				name,
				uuid,
				system_clock::now(),
				parent_ID,
				false,
				0min,
				false
				),
		expanded(expanded),
		running(running),
		time(time),
		total_time(total_time_)
{
}

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
