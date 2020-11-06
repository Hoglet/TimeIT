#include "ExtendedTask.h"

namespace DB
{

bool ExtendedTask::getExpanded() const
{
	return expanded;
}
bool ExtendedTask::getRunning() const
{
	return running;
}
void ExtendedTask::setExpanded(bool op)
{
	expanded = op;
}

ExtendedTask::ExtendedTask(int op_ID, int op_parentID, std::string op_name, int op_time, bool op_expanded, bool op_running, int op_totalTime)
{
	ID_ = op_ID;
	parentID_ = op_parentID;
	name_ = op_name;
	totalTime = op_totalTime;
	expanded = op_expanded;
	running = op_running;
	time = op_time;
}

int ExtendedTask::getTotalTime() const
{
	return totalTime;
}

void ExtendedTask::setTotalTime(int op_totalTime)
{
	totalTime = op_totalTime;
}

int ExtendedTask::getTime() const
{
	return time;
}

void ExtendedTask::setTime(int op_time)
{
	time = op_time;
}
}
