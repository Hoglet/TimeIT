
#include <libtimeit/datatypes/time_entry.h>
#include <utility>

namespace libtimeit
{

time_entry::time_entry(
		time_id                  op_id,
		task_id                  owner_id,
		time_point<system_clock> op_start,
		seconds                  op_duration,
		time_entry_state         op_state,
		time_point<system_clock> op_changed,
		string                   op_comment)
		:
		id( op_id ),
		start( op_start ),
		duration( op_duration ),
		state( op_state),
		changed( op_changed ),
		owner_id( owner_id ),
		comment( op_comment )
{
}


time_entry::time_entry(
		task_id owner,
		time_point<system_clock> op_start,
		seconds                  op_duration,
		time_entry_state         op_state
		)
		:
		time_entry(
				time_id(uuid()) ,
				owner,
				op_start,
				op_duration,
				op_state,
				system_clock::now(),
				{})
{
}



time_entry::time_entry(
		task_id owner,
		time_point<system_clock>  op_start,
		seconds                   op_duration)
		:
		time_entry(
				time_id(uuid()) ,
				owner,
				op_start,
				op_duration,
				stopped,
				system_clock::now(),
				{})
{
}


time_entry time_entry::with_start( time_point<system_clock> start_point) const
{
	if ( start == start_point )
	{
		return *this;
	}
	return {
		id,
		owner_id,
		start_point,
		duration,
		state,
		system_clock::now(),
		comment};
}

time_entry time_entry::with_duration( seconds op_duration) const
{
	if ( duration == op_duration )
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start,
			op_duration,
			state,
			system_clock::now(),
			comment};
}


time_entry time_entry::with( time_entry_state new_state) const
{
	if (state == new_state)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start,
			duration,
			new_state,
			system_clock::now(),
			comment};
}

time_entry time_entry::with_comment( string new_comment)
{
	if (comment == new_comment)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start,
			duration,
			state,
			system_clock::now(),
			new_comment
			};
}
}
