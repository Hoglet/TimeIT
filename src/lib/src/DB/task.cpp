
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
			id,
			completed,
			time(nullptr),
			parent_id,
			deleted,
			idle,
			quiet);

}

task task::with_parent(optional<task_id> new_parent_id) const
{
	if( parent_id == new_parent_id)
	{
		return *this;
	}
	return 	task(
			name,
			id,
			completed,
			time(nullptr),
			new_parent_id,
			deleted,
			idle,
			quiet);
}

task::task(string name_, optional<task_id> parent_ID_)
	:
		task(
			name_,
			task_id(),
			false,
			time( nullptr ),
			parent_ID_,
			false,
			0,
			false)
{

}

task::task(
		string               name_,
		task_id              uuid_,
		bool                 completed_,
		time_t               last_change_,
		optional<task_id>    parent_uuid_,
		bool                 deleted_,
		unsigned             idle_,
		bool                 quiet_)
	:
		name(move(name_)),
		id( move( uuid_)),
		parent_id( move( parent_uuid_)),
		completed(completed_),
		deleted(deleted_),
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
			id,
			new_completed,
			time(nullptr),
			parent_id,
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
			id,
			completed,
			time(nullptr),
			parent_id,
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
			id,
			completed,
			time(nullptr),
			parent_id,
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
			id,
			completed,
			time(nullptr),
			parent_id,
			deleted,
			idle,
			new_quiet);
}

bool operator==(const task &op1, const task &op2)
{
	return (op1.name         == op2.name &&
			op1.id == op2.id &&
			op1.parent_id == op2.parent_id &&
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
