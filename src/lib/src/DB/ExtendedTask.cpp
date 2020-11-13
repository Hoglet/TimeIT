#include "DB/ExtendedTask.h"

namespace DB
{

bool ExtendedTask::expanded() const
{
	return expanded_;
}

bool ExtendedTask::running() const
{
	return running_;
}

ExtendedTask::ExtendedTask(
		int op_ID,
		int op_parentID,
		const std::string& op_name,
		int op_time,
		bool op_expanded,
		bool op_running,
		int op_totalTime)
		:
		Task(op_name, op_parentID),
		totalTime_(op_totalTime),
		expanded_(op_expanded),
		running_(op_running),
		time_(op_time)
{
	ID_ = op_ID;
}

int ExtendedTask::totalTime() const
{
	return totalTime_;
}

int ExtendedTask::time() const
{
	return time_;
}

}
