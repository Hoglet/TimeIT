
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

task::task( string op_name, optional<task_id> op_parent_id)
	:
		task(
			op_name,
			task_id(),
			system_clock::now(),
			op_parent_id,
			false,
			0min,
			false)
{

}

task::task(
		string_view              op_name,
		task_id                  op_uuid,
		time_point<system_clock> op_last_change,
		optional<task_id>        op_parent_uuid,
		bool                     op_deleted,
		minutes                  op_idle,
		bool                     op_quiet)
	:
		name( op_name ),
		id( op_uuid),
		parent_id(  op_parent_uuid),
		deleted( op_deleted),
		last_changed( op_last_change),
		idle( op_idle),
		quiet( op_quiet)
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

bool operator==( const task &op_1, const task &op_2)
{
	return ( op_1.name == op_2.name &&
			 op_1.id == op_2.id &&
			 op_1.parent_id == op_2.parent_id &&
			 op_1.deleted == op_2.deleted &&
			 op_1.last_changed == op_2.last_changed &&
			 op_1.idle == op_2.idle &&
			 op_1.quiet == op_2.quiet
	);
}

bool operator!=( const task &op_1, const task &op_2)
{
	return !( op_1 == op_2);
}

}
