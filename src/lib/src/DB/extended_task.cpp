#include "libtimeit/db/extended_task.h"

namespace libtimeit
{

Extended_task::Extended_task(
		int           op_ID,
		int           op_parent_ID,
		const string& op_name,
		int           op_time,
		bool          op_expanded,
		bool          op_running,
		int           op_total_time)
		:
		Task(
				op_name,
				op_parent_ID,
				UUID(),
				false,
				op_ID,
				0,
				{},
				false,
				0,
				false
				),
		total_time_(op_total_time),
		expanded_(op_expanded),
		running_(op_running),
		time_(op_time)
{
}

int Extended_task::total_time() const
{
	return total_time_;
}

}
