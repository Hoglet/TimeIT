
#include <stdexcept>
#include <utility>
#include <libtimeit/db/task.h>

namespace libtimeit
{


task task::with_name(string new_name) const
{
	if(name == new_name)
	{
		return *this;
	}
	return 	task(
			new_name,
			parent_id,
			uuid,
			completed,
			id,
			time(nullptr),
			parent_uuid,
			deleted,
			idle,
			quiet);

}

task task::with_parent(Task_id new_parent_ID) const
{
	if(parent_id == new_parent_ID)
	{
		return *this;
	}
	return 	task(
			name,
			new_parent_ID,
			uuid,
			completed,
			id,
			time(nullptr),
			parent_uuid,
			deleted,
			idle,
			quiet);
}

task::task(string name_, Task_id parent_ID_)
	:
		task(
			name_,
			parent_ID_,
			UUID(),
			false,
			0,
			time( nullptr ),
			{},
			false,
			0,
			false)
{

}

task::task(
		string               name_,
		Task_id              parentID_,
		class UUID           uuid_,
		bool                 completed_,
		Task_id              ID_,
		time_t               last_change_,
		optional<class UUID> parent_uuid_,
		bool                 deleted_,
		unsigned             idle_,
		bool                 quiet_)
	:
		id(ID_),
		name(move(name_)),
		uuid(move(uuid_)),
		parent_uuid(move(parent_uuid_)),
		completed(completed_),
		deleted(deleted_),
		parent_id(parentID_),
		last_changed(last_change_),
		idle(idle_),
		quiet(quiet_)
{

}



task task::with_completed(bool new_completed) const
{
	if(completed == new_completed )
	{
		return *this;
	}
	return 	task(
			name,
			parent_id,
			uuid,
			new_completed,
			id,
			time(nullptr),
			parent_uuid,
			deleted,
			idle,
			quiet);
}

task task::with_deleted(bool new_deleted) const
{
	if(deleted == new_deleted )
	{
		return *this;
	}
	return 	task(
			name,
			parent_id,
			uuid,
			completed,
			id,
			time(nullptr),
			parent_uuid,
			new_deleted,
			idle,
			quiet);
}
task task::with_idle(unsigned new_idle) const
{
	if(idle == new_idle )
	{
		return *this;
	}
	return 	task(
			name,
			parent_id,
			uuid,
			completed,
			id,
			time(nullptr),
			parent_uuid,
			deleted,
			new_idle,
			quiet);
}

task task::with_quiet(bool new_quiet) const
{
	if(quiet == new_quiet )
	{
		return *this;
	}
	return 	task(
			name,
			parent_id,
			uuid,
			completed,
			id,
			time(nullptr),
			parent_uuid,
			deleted,
			idle,
			new_quiet);
}

bool operator==(const task &op1, const task &op2)
{
	return (op1.name         == op2.name &&
			op1.id == op2.id &&
			op1.parent_id == op2.parent_id &&
			op1.uuid         == op2.uuid &&
			op1.parent_uuid == op2.parent_uuid &&
			op1.completed    == op2.completed &&
			op1.deleted      == op2.deleted &&
			op1.last_changed == op2.last_changed &&
			op1.idle         == op2.idle &&
			op1.quiet        == op2.quiet

	);
}

bool operator!=(const task &op1, const task &op2)
{
	return !(op1 == op2);
}

}
