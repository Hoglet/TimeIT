#include "TimeEntry.h"

namespace DB
{
TimeEntry::TimeEntry(int64_t op_id, std::string op_uuid, int64_t op_taskID, std::string op_taskUUID, time_t op_start,
		time_t op_stop, bool op_deleted, bool op_running, time_t op_changed)
{
	id = op_id;
	taskID = op_taskID;
	start = op_start;
	stop = op_stop;
	deleted = op_deleted;
	running = op_running;
	changed = op_changed;
	uuid = op_uuid;
	taskUUID = op_taskUUID;
}

int64_t TimeEntry::getID() const
{
	return id;
}
time_t TimeEntry::getStart() const
{
	return start;
}
void TimeEntry::setStart(time_t op_start)
{
	if(start != op_start)
	{
		start = op_start;
		changed = time(0);
	}
}

time_t TimeEntry::getStop() const
{
	return stop;
}

void TimeEntry::setStop(time_t op_stop)
{
	if(stop!=op_stop)
	{
		stop = op_stop;
		changed = time(0);
	}
}

int64_t TimeEntry::getTaskID() const
{
	return taskID;
}
std::string TimeEntry::getUUID() const
{
	return uuid;
}

std::string TimeEntry::getTaskUUID() const
{
	return taskUUID;
}

bool TimeEntry::getRunning() const
{
	return running;
}

time_t TimeEntry::getLastChanged() const
{
	return changed;
}

bool TimeEntry::getDeleted() const
{
	return deleted;
}
void TimeEntry::setDeleted(bool state)
{
	if (deleted != state)
	{
		deleted = state;
		changed = time(0);
	}
}

bool operator==(const TimeEntry &op1, const TimeEntry &op2)
{
	return (op1.id == op2.id && op1.changed == op2.changed && op1.deleted == op2.deleted && op1.running == op2.running
			&& op1.start == op2.start && op1.stop == op2.stop && op1.taskID == op2.taskID && op1.uuid == op2.uuid
			&& op1.taskUUID == op2.taskUUID);
}

bool operator!=(const TimeEntry &op1, const TimeEntry &op2)
{
	return !(op1 == op2);
}

}
