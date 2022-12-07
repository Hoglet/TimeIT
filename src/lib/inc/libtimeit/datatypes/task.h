#ifndef TASK_H
#define TASK_H

#include <string>
#include <optional>
#include <libtimeit/datatypes/uuid.h>
#include <libtimeit/datatypes/task_id.h>
#include <chrono>


namespace libtimeit
{
class task_accessor;

using namespace std;
using namespace std::chrono;

struct [[nodiscard]]  task
{
	const string                    name;
	const task_id                   id;
	const optional<task_id>         parent_id;
	const bool                      deleted      {false};
	const time_point<system_clock>  last_changed;
	const minutes                   idle         {0min};
	const bool                      quiet        {false};



	explicit task( string op_name, optional<task_id> op_parent_id = {});
	task(
			string_view               op_name,
			task_id                   op_uuid,
			time_point<system_clock>  op_last_change,
			optional<task_id>         op_parent_uuid,
			bool                      op_deleted,
			minutes                   op_idle,
			bool                      op_quiet);

	task       with_name(const string& /*new_name*/)        const;
	task       with_parent(const optional<task_id>& /*id*/) const;
	task       with_deleted(bool  /*deleted*/)       const;
	task       with_idle(minutes /*idle*/)          const;
	task       with_quiet(bool /*quiet*/)            const;
};

extern bool operator==( const task &op_1, const task &op_2);
extern bool operator!=( const task &op_1, const task &op_2);
}

#endif
