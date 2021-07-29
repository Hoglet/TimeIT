#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

extended_task::extended_task(
		task_id           uuid,
		optional<task_id> parent_ID,
		const string&     name,
		int               time,
		bool              expanded,
		bool              running,
		int               total_time_)
		:
		task(
				name,
				uuid,
				0,
				parent_ID,
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

extended_task::extended_task(
		const task&       source,
		int               time,
		int               total_time_,
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
