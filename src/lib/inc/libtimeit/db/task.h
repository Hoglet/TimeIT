#ifndef TIMEIT_TASK_H
#define TIMEIT_TASK_H

#include <string>
#include <optional>
#include <libtimeit/db/data_types.h>
#include <libtimeit/datatypes/uuid.h>


namespace libtimeit
{
class task_accessor;

using namespace std;

struct [[nodiscard]]  task
{
	const Task_id              id           {0};
	const string               name;
	const class UUID           uuid;
	const optional<class UUID> parent_uuid;
	const bool                 completed    {false};
	const bool                 deleted      {false};
	const Task_id              parent_id    {0};
	const time_t               last_changed {0};
	const unsigned             idle         {0};
	const bool                 quiet        {false};



	task(string name_, Task_id parent_ID = 0);
	task(
			string         name_,
			Task_id        parentID_,
			UUID           uuid_,
			bool           completed_,
			Task_id        ID_,
			time_t         last_change_,
			optional<UUID> parent_uuid_,
			bool           deleted_,
			unsigned       idle_,
			bool           quiet_);

	task       with_name(string /*new_name*/)     const;
	task       with_parent(Task_id /*id*/)        const;
	task       with_completed(bool /*completed*/) const;
	task       with_deleted(bool  /*deleted*/)    const;
	task       with_idle(unsigned /*idle*/)       const;
	task       with_quiet(bool /*quiet*/)         const;
};

extern bool operator==(const task &op1, const task &op2);
extern bool operator!=(const task &op1, const task &op2);
}

#endif