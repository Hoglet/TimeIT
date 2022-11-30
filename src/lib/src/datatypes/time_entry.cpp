
#include <libtimeit/datatypes/time_entry.h>
#include <utility>

namespace libtimeit
{

time_entry::time_entry(
		time_id                  op_id,
		task_id                  owner_id,
		time_point<system_clock> op_start,
		time_point<system_clock> op_stop,
		time_entry_state         op_state,
		time_point<system_clock> op_changed,
		string                   op_comment)
		:
		id( op_id ),
		start( op_start ),
		stop( op_stop ),
		state( op_state),
		changed( op_changed ),
		owner_id(std::move( owner_id)),
		comment(std::move( op_comment))
		{
		}



time_entry::time_entry(
		task_id owner,
		time_point<system_clock>  op_start,
		time_point<system_clock>  op_stop)
		:
		time_entry(
				time_id(uuid()) ,
				owner,
				op_start,
				op_stop,
				STOPPED,
				system_clock::now(),
				{})
{
}

time_entry::time_entry(
		task_id owner,
		time_point<system_clock> op_start,
		time_point<system_clock> op_stop,
		time_entry_state         op_state
		)
		:
		time_entry(
				time_id(uuid()) ,
				owner,
				op_start,
				op_stop,
				op_state,
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
		stop,
		state,
		system_clock::now(),
		comment};
}

time_entry time_entry::with_stop( time_point<system_clock> stop_point) const
{
	if( stop == stop_point )
	{
		return *this;
	}
	return {
		id,
		owner_id,
		start,
		stop_point,
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
			stop,
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
			stop,
			state,
			system_clock::now(),
			new_comment
			};
}

bool operator==( const time_entry &op_1, const time_entry &op_2 )
{
	return (
			op_1.changed == op_2.changed &&
			op_1.state == op_2.state &&
			op_1.start == op_2.start &&
			op_1.stop == op_2.stop &&
			op_1.id == op_2.id &&
			op_1.owner_id == op_2.owner_id &&
			op_1.comment == op_2.comment
			);
}

bool operator!=( const time_entry &op_1, const time_entry &op_2 )
{
	return !( op_1 == op_2);
}

}
