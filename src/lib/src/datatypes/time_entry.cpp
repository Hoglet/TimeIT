
#include <libtimeit/datatypes/time_entry.h>
#include <utility>

namespace libtimeit
{
time_entry::time_entry(
		time_id               op_id,
		task_id               owner_id,
		time_t                op_start,
		time_t                op_stop,
		time_entry_state      op_state,
		time_t                op_changed,
		string                op_comment)
		:
		id( move( op_id)),
		start( op_start),
		stop( op_stop),
		state( op_state),
		changed( op_changed),
		owner_id(std::move( owner_id)),
		comment(std::move( op_comment))
{
}



time_entry::time_entry(
		task_id owner,
		time_t  op_start,
		time_t  op_stop)
			:
		id( uuid() ),
		start( op_start ),
		stop( op_stop ),
		state(STOPPED ),
		changed(time(nullptr)),
		owner_id(std::move( owner)),
		comment()
{
}

time_entry::time_entry(
		task_id          owner_id,
		time_t           op_start,
		time_t           op_stop,
		time_entry_state op_state)
		:
		id( uuid() ),
		start( op_start ),
		stop( op_stop ),
		state( op_state ),
		changed( time(nullptr) ),
		owner_id(std::move( owner_id)),
		comment()
{
}



time_entry time_entry::with_start( time_t op_start) const
{
	if ( start == op_start)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			op_start,
			stop,
			state,
			time(nullptr),
			comment};
}

time_entry time_entry::with_stop( time_t op_stop) const
{
	if( stop == op_stop)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start,
			op_stop,
			state,
			time(nullptr),
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
			time(nullptr),
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
			time(nullptr),
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
