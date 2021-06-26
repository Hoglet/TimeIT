
#include <libtimeit/db/time_entry.h>
#include <utility>

namespace libtimeit
{
Time_entry::Time_entry(
		Time_id               id_,
		class UUID            uuid_,
		int64_t               task_ID_,
		optional<class UUID>  taskUUID_,
		time_t                start_,
		time_t                stop_,
		Time_entry_state      state_,
		time_t                changed_,
		string                comment_)
		:
		id(id_),
		uuid(move(move(uuid_))),
		task_id(task_ID_),
		start(start_),
		stop(stop_),
		state(state_),
		changed(changed_),
		task_uuid(move(move(taskUUID_))),
		comment(comment_)
{
}



Time_entry::Time_entry(
		Task_id task_ID_,
		time_t  start_,
		time_t  stop_)
			:
		id(0),
		uuid(UUID() ),
		task_id(task_ID_ ),
		start(start_ ),
		stop(stop_ ),
		state(STOPPED ),
		changed(time(nullptr)),
		comment()
{
}

Time_entry::Time_entry(
		Task_id          task_ID_,
		time_t           start_,
		time_t           stop_,
		Time_entry_state state_)
		:
		id(0),
		uuid(UUID() ),
		task_id(task_ID_ ),
		start(start_ ),
		stop(stop_ ),
		state(state_ ),
		changed(time(nullptr)),
		comment()
{
}



Time_entry Time_entry::with_start(time_t start_) const
{
	if (start == start_)
	{
		return *this;
	}
	return Time_entry(
			id,
			uuid,
			task_id,
			task_uuid,
			start_,
			stop,
			state,
			time(nullptr),
			comment);
}

Time_entry Time_entry::with_stop(time_t stop_) const
{
	if(stop == stop_)
	{
		return *this;
	}
	return Time_entry(
			id,
			uuid,
			task_id,
			task_uuid,
			start,
			stop_,
			state,
			time(nullptr),
			comment);
}


Time_entry Time_entry::with( Time_entry_state new_state) const
{
	if (state == new_state)
	{
		return *this;
	}
	return Time_entry(
			id,
			uuid,
			task_id,
			task_uuid,
			start,
			stop,
			new_state,
			time(nullptr),
			comment);
}

Time_entry Time_entry::with_comment(string new_comment)
{
	if (comment == new_comment)
	{
		return *this;
	}
	return Time_entry(
			id,
			uuid,
			task_id,
			task_uuid,
			start,
			stop,
			state,
			time(nullptr),
			new_comment
			);
}

bool operator==(const Time_entry &op1, const Time_entry &op2)
{
	return (
			op1.id        == op2.id        &&
			op1.changed   == op2.changed   &&
			op1.state     == op2.state     &&
			op1.start     == op2.start     &&
			op1.stop      == op2.stop      &&
			op1.task_id   == op2.task_id   &&
			op1.uuid      == op2.uuid      &&
			op1.task_uuid == op2.task_uuid &&
			op1.comment   == op2.comment
			);
}

bool operator!=(const Time_entry &op1, const Time_entry &op2)
{
	return !(op1 == op2);
}

}
