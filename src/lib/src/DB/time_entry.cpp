
#include <libtimeit/db/time_entry.h>

namespace libtimeit
{
Time_entry::Time_entry(
		Time_ID               id,
		class UUID            uuid,
		int64_t               task_ID,
		optional<class UUID>  taskUUID,
		time_t                start,
		time_t                stop,
		bool                  deleted,
		Time_entry_state      state,
		time_t                changed)
		:
		id_(id),
		uuid_(uuid),
		task_ID_(task_ID),
		task_UUID_(taskUUID),
		start_(start),
		stop_(stop),
		deleted_(deleted),
		state_(state),
		changed_(changed)
{
}



Time_entry::Time_entry(
				Task_ID task_ID,
				time_t  start,
				time_t  stop)
			:
			id_(0),
			uuid_( UUID() ),
			task_ID_( task_ID ),
			task_UUID_( {} ),
			start_(	start ),
			stop_( stop ),
			deleted_( false ),
			state_( STOPPED ),
			changed_( time(nullptr) )
{
}


int64_t Time_entry::ID() const
{
	return id_;
}
time_t Time_entry::start() const
{
	return start_;
}

Time_entry Time_entry::with_start(time_t start) const
{
	if (start_ == start)
	{
		return *this;
	}
	else
	{
		Time_entry returnValue(*this);
		returnValue.start_   = start;
		returnValue.changed_ = time(nullptr);
		return returnValue;
	}
}

time_t Time_entry::stop() const
{
	return stop_;
}

Time_entry Time_entry::with_stop(time_t stop) const
{
	if(stop_ == stop)
	{
		return *this;
	}
	else
	{
		auto returnValue(*this);
		returnValue.stop_ = stop;
		returnValue.changed_ = time(nullptr);
		return returnValue;
	}
}

int64_t Time_entry::task_ID() const
{
	return task_ID_;
}
class UUID Time_entry::get_UUID() const
{
	return uuid_;
}

optional<class UUID> Time_entry::task_UUID() const
{
	return task_UUID_;
}



bool Time_entry::running() const
{
	return state_ == RUNNING;
}

time_t Time_entry::changed() const
{
	return changed_;
}

bool Time_entry::deleted() const
{
	return deleted_;
}
Time_entry Time_entry::with_deleted(bool state) const
{
	if (deleted_ == state)
	{
		return *this;
	}
	else
	{
		auto returnValue(*this);
		returnValue.deleted_ = state;
		returnValue.changed_ = time(nullptr);
		return returnValue;
	}
}

Time_entry Time_entry::with( Time_entry_state new_state) const
{
	if ( state_ == new_state)
	{
		return *this;
	}
	else
	{
		auto new_entry(*this);
		new_entry.state_   = new_state;
		new_entry.changed_ = time(nullptr);
		return new_entry;
	}
}

bool operator==(const Time_entry &op1, const Time_entry &op2)
{
	return (op1.id_ == op2.id_ && op1.changed_ == op2.changed_ && op1.deleted_ == op2.deleted_ && op1.state_ == op2.state_
			&& op1.start_ == op2.start_ && op1.stop_ == op2.stop_ && op1.task_ID_ == op2.task_ID_ && op1.uuid_ == op2.uuid_
			&& op1.task_UUID_ == op2.task_UUID_);
}

bool operator!=(const Time_entry &op1, const Time_entry &op2)
{
	return !(op1 == op2);
}

}
