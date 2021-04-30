
#include <libtimeit/db/time_entry.h>

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
		time_t                changed_)
		:
		ID(id_),
		uuid(uuid_),
		task_ID(task_ID_),
		task_uuid(taskUUID_),
		start(start_),
		stop(stop_),
		state(state_),
		changed(changed_)
{
}



Time_entry::Time_entry(
		Task_id task_ID_,
		time_t  start_,
		time_t  stop_)
			:
		ID(0),
		uuid(UUID() ),
		task_ID(task_ID_ ),
		task_uuid({} ),
		start(start_ ),
		stop(stop_ ),
		state(STOPPED ),
		changed(time(nullptr) )
{
}

Time_entry::Time_entry(
		Task_id          task_ID_,
		time_t           start_,
		time_t           stop_,
		Time_entry_state state_)
		:
		ID(0),
		uuid(UUID() ),
		task_ID(task_ID_ ),
		task_uuid({} ),
		start(start_ ),
		stop(stop_ ),
		state(state_ ),
		changed(time(nullptr) )
{
}



Time_entry Time_entry::with_start(time_t start_) const
{
	if (start == start_)
	{
		return *this;
	}
	else
	{
		return Time_entry(
				ID,
				uuid,
				task_ID,
				task_uuid,
				start_,
				stop,
				state,
				time(nullptr));
	}
}

Time_entry Time_entry::with_stop(time_t stop_) const
{
	if(stop == stop_)
	{
		return *this;
	}
	else
	{
		return Time_entry(
				ID,
				uuid,
				task_ID,
				task_uuid,
				start,
				stop_,
				state,
				time(nullptr));
	}
}


Time_entry Time_entry::with( Time_entry_state new_state) const
{
	if (state == new_state)
	{
		return *this;
	}
	else
	{
		return Time_entry(
				ID,
				uuid,
				task_ID,
				task_uuid,
				start,
				stop,
				new_state,
				time(nullptr));
	}
}

bool operator==(const Time_entry &op1, const Time_entry &op2)
{
	return (op1.ID == op2.ID && op1.changed == op2.changed && op1.state == op2.state
			&& op1.start == op2.start && op1.stop == op2.stop && op1.task_ID == op2.task_ID && op1.uuid == op2.uuid
			&& op1.task_uuid == op2.task_uuid);
}

bool operator!=(const Time_entry &op1, const Time_entry &op2)
{
	return !(op1 == op2);
}

}
