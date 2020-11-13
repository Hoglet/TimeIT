#include "DB/TimeEntry.h"

namespace DB
{
TimeEntry::TimeEntry(
		int64_t               op_id,
		class UUID            op_uuid,
		int64_t               op_taskID,
		optional<class UUID>  op_taskUUID,
		time_t                op_start,
		time_t                op_stop,
		bool                  op_deleted,
		bool                  op_running,
		time_t                op_changed)
		:
		id_(op_id),
		uuid_(op_uuid),
		taskID_(op_taskID),
		taskUUID_(op_taskUUID),
		start_(op_start),
		stop_(op_stop),
		deleted_(op_deleted),
		running_(op_running),
		changed_(op_changed)
{
}

int64_t TimeEntry::ID() const
{
	return id_;
}
time_t TimeEntry::start() const
{
	return start_;
}

TimeEntry TimeEntry::withStart(time_t op_start) const
{
	if (start_ == op_start)
	{
		return *this;
	}
	else
	{
		TimeEntry returnValue(*this);
		returnValue.start_ = op_start;
		returnValue.changed_ = time(nullptr);
		return returnValue;
	}
}

time_t TimeEntry::stop() const
{
	return stop_;
}

TimeEntry TimeEntry::withStop(time_t op_stop) const
{
	if(stop_ == op_stop)
	{
		return *this;
	}
	else
	{
		auto returnValue(*this);
		returnValue.stop_ = op_stop;
		returnValue.changed_ = time(nullptr);
		return returnValue;
	}
}

int64_t TimeEntry::taskID() const
{
	return taskID_;
}
class UUID TimeEntry::getUUID() const
{
	return uuid_;
}

optional<class UUID> TimeEntry::taskUUID() const
{
	return taskUUID_;
}

bool TimeEntry::running() const
{
	return running_;
}

time_t TimeEntry::changed() const
{
	return changed_;
}

bool TimeEntry::deleted() const
{
	return deleted_;
}
TimeEntry TimeEntry::withDeleted(bool state) const
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

bool operator==(const TimeEntry &op1, const TimeEntry &op2)
{
	return (op1.id_ == op2.id_ && op1.changed_ == op2.changed_ && op1.deleted_ == op2.deleted_ && op1.running_ == op2.running_
			&& op1.start_ == op2.start_ && op1.stop_ == op2.stop_ && op1.taskID_ == op2.taskID_ && op1.uuid_ == op2.uuid_
			&& op1.taskUUID_ == op2.taskUUID_);
}

bool operator!=(const TimeEntry &op1, const TimeEntry &op2)
{
	return !(op1 == op2);
}

}
