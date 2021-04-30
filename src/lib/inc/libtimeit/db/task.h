#ifndef TIMEIT_TASK_H
#define TIMEIT_TASK_H

#include <string>
#include <optional>
#include <libtimeit/db/data_types.h>
#include <libtimeit/db/uuid.h>


namespace libtimeit
{
class Task_accessor;

using namespace std;

struct [[nodiscard]]  Task
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



	Task(string name_, Task_id parent_ID = 0);
	Task(
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

	virtual ~Task() = default;

	Task       with_name(string /*new_name*/)     const;
	Task       with_parent(Task_id /*id*/)        const;
	Task       with_completed(bool /*completed*/) const;
	Task       with_deleted(bool  /*deleted*/)    const;
	Task       with_idle(unsigned /*idle*/)       const;
	Task       with_quiet(bool /*quiet*/)         const;

	friend bool       operator==(const Task &op1, const Task &op2);
	friend bool       operator!=(const Task &op1, const Task &op2);

};

extern bool operator==(const Task &op1, const Task &op2);
extern bool operator!=(const Task &op1, const Task &op2);
}

#endif