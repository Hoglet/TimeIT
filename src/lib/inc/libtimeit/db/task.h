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

struct Task
{
	const Task_ID              ID          {0};
	const string               name;
	const class UUID           uuid;
	const optional<class UUID> parent_uuid;
	const bool                 completed    {false};
	const bool                 deleted      {false};
	const Task_ID              parent_ID    {0};
	const time_t               last_changed {0};



	Task(string name_, Task_ID parent_ID = 0);
	Task(
		string         name_,
		Task_ID        parentID_,
		UUID           uuid_,
		bool           completed_,
		Task_ID        ID_,
		time_t         last_change_,
		optional<UUID> parent_uuid_,
		bool           deleted_);

	virtual ~Task() = default;

	Task       with_name(string new_name) const;
	Task       with_parent(Task_ID)              const;
	Task       with_completed(bool)              const;
	Task       with_deleted(bool)                const;

	friend bool       operator==(const Task &op1, const Task &op2);
	friend bool       operator!=(const Task &op1, const Task &op2);

};

extern bool operator==(const Task &op1, const Task &op2);
extern bool operator!=(const Task &op1, const Task &op2);
}

#endif