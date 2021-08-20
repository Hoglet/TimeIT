
#include <stdexcept>
#include <utility>
#include <libtimeit/datatypes/task.h>

namespace libtimeit
{


task task::with_name(const string& new_name) const
{
	if(name == new_name)
	{
		return *this;
	}
	return 	{
			new_name,
			id,
			system_clock::now(),
			parent_id,
			deleted,
			idle,
			quiet };

}

task task::with_parent(const optional<task_id>& new_parent_id) const
{
	if( parent_id == new_parent_id)
	{
		return *this;
	}
	return 	{
			name,
			id,
			system_clock::now(),
			new_parent_id,
			deleted,
			idle,
			quiet};
}

task::task(string name_, optional<task_id> parent_ID_)
	:
		task(
			std::move(name_),
			task_id(),
			system_clock::now(),
			std::move(parent_ID_),
			false,
			0min,
			false)
{

}

task::task(
		string                   name_,
		task_id                  uuid_,
		time_point<system_clock> last_change_,
		optional<task_id>        parent_uuid_,
		bool                     deleted_,
		minutes                  idle_,
		bool                     quiet_)
	:
		name(move(name_)),
		id( move( uuid_)),
		parent_id( move( parent_uuid_)),
		deleted(deleted_),
		last_changed(last_change_),
		idle(idle_),
		quiet(quiet_)
{

}



task task::with_deleted(bool new_deleted) const
{
	if(deleted == new_deleted )
	{
		return *this;
	}
	return 	{
			name,
			id,
			system_clock::now(),
			parent_id,
			new_deleted,
			idle,
			quiet};
}
task task::with_idle(minutes new_idle) const
{
	if(idle == new_idle )
	{
		return *this;
	}
	return 	{
			name,
			id,
			system_clock::now(),
			parent_id,
			deleted,
			new_idle,
			quiet};
}

task task::with_quiet(bool new_quiet) const
{
	if(quiet == new_quiet )
	{
		return *this;
	}
	return 	{
			name,
			id,
			system_clock::now(),
			parent_id,
			deleted,
			idle,
			new_quiet};
}

bool operator==(const task &op1, const task &op2)
{
	return (op1.name         == op2.name &&
			op1.id           == op2.id &&
			op1.parent_id    == op2.parent_id &&
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
