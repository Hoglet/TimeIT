
#include <libtimeit/datatypes/time_entry.h>
#include <utility>

namespace libtimeit
{
Time_entry::Time_entry(
		time_id               id_,
		task_id               owner_id,
		time_t                start_,
		time_t                stop_,
		Time_entry_state      state_,
		time_t                changed_,
		string                comment_)
		:
		id( move( id_)),
		start(start_),
		stop(stop_),
		state(state_),
		changed(changed_),
		owner_id(std::move( owner_id)),
		comment(std::move(comment_))
{
}



Time_entry::Time_entry(
		task_id owner,
		time_t  start_,
		time_t  stop_)
			:
		id( uuid() ),
		start(start_ ),
		stop(stop_ ),
		state(STOPPED ),
		changed(time(nullptr)),
		owner_id(std::move( owner)),
		comment()
{
}

Time_entry::Time_entry(
		task_id          owner_id,
		time_t           start_,
		time_t           stop_,
		Time_entry_state state_)
		:
		id( uuid() ),
		start( start_ ),
		stop( stop_ ),
		state( state_ ),
		changed( time(nullptr) ),
		owner_id(std::move( owner_id)),
		comment()
{
}



Time_entry Time_entry::with_start(time_t start_) const
{
	if (start == start_)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start_,
			stop,
			state,
			time(nullptr),
			comment};
}

Time_entry Time_entry::with_stop(time_t stop_) const
{
	if(stop == stop_)
	{
		return *this;
	}
	return {
			id,
			owner_id,
			start,
			stop_,
			state,
			time(nullptr),
			comment};
}


Time_entry Time_entry::with( Time_entry_state new_state) const
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

Time_entry Time_entry::with_comment(string new_comment)
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

bool operator==(const Time_entry &op1, const Time_entry &op2)
{
	return (
			op1.changed   == op2.changed &&
			op1.state     == op2.state &&
			op1.start     == op2.start &&
			op1.stop      == op2.stop &&
			op1.id == op2.id &&
			op1.owner_id == op2.owner_id &&
			op1.comment   == op2.comment
			);
}

bool operator!=(const Time_entry &op1, const Time_entry &op2)
{
	return !(op1 == op2);
}

}
