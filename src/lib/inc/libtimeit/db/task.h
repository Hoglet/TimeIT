#ifndef TIMEIT_TASK_H
#define TIMEIT_TASK_H

#include <string>
#include <optional>
#include <libtimeit/db/data_types.h>
#include <libtimeit/datatypes/uuid.h>
#include <libtimeit/datatypes/task_id.h>


namespace libtimeit
{
class task_accessor;

using namespace std;

struct [[nodiscard]]  task
{
	const string               name;
	const task_id              id;
	const optional<task_id>    parent_id;
	const bool                 deleted      {false};
	const time_t               last_changed {0};
	const unsigned             idle         {0};
	const bool                 quiet        {false};



	task(string name_, optional<task_id> parent_ID = {});
	task(
			string            name_,
			task_id           uuid_,
			time_t            last_change_,
			optional<task_id> parent_uuid_,
			bool              deleted_,
			unsigned          idle_,
			bool              quiet_);

	task       with_name(string /*new_name*/)        const;
	task       with_parent(optional<task_id> /*id*/) const;
	task       with_deleted(bool  /*deleted*/)       const;
	task       with_idle(unsigned /*idle*/)          const;
	task       with_quiet(bool /*quiet*/)            const;
};

extern bool operator==(const task &op1, const task &op2);
extern bool operator!=(const task &op1, const task &op2);
}

#endif