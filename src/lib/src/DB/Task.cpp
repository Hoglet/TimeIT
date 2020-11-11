#include "Task.h"
#include <stdexcept>
#include <UUIDTool.h>

namespace DB
{

Task::Task()
{
	lastChanged_ = time(nullptr);
}

Task::~Task()
{
}

string Task::name() const
{
	return name_;
}

Task Task::withName(const string& newName) const
{
	if(name_ == newName)
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.name_ = newName;
		newTask.lastChanged_ = time(nullptr);
		return newTask;
	}
}

int64_t Task::ID() const
{
	return ID_;
}

int64_t Task::parentID() const
{
	return parentID_;
}

Task Task::withParent(int64_t op1) const
{
	if(parentID_ == op1)
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.parentID_ = op1;
		newTask.lastChanged_ = time(nullptr);
		return newTask;
	}
}

Task::Task(const string &op_name, int64_t op_parentID): Task(op_name, op_parentID, UUID(), false, 0, 0, {}, false)
{

}

Task::Task(
	const std::string&   op_name,
	int64_t              op_parentID,
	class UUID           op_uuid,
	bool                 op_completed,
	int64_t              op_ID,
	time_t               op_lastChange,
	optional<class UUID> op_parentUUID,
	bool                 op_deleted)
	:
	uuid_(op_uuid),
	parentUuid_(op_parentUUID)
{
	if (op_lastChange == 0)
	{
		lastChanged_ = time(nullptr);
	}
	else
	{
		lastChanged_ = op_lastChange;
	}
	ID_ = op_ID;
	parentID_ = op_parentID;
	name_ = op_name;
	completed_ = op_completed;
	deleted_ = op_deleted;
}

UUID Task::getUUID() const
{
	return uuid_;
}

optional<class UUID> Task::parentUUID() const
{
	return parentUuid_;
}

bool Task::completed() const
{
	return completed_;
}
time_t Task::lastChanged() const
{
	return lastChanged_;
}

Task Task::withCompleted(bool state) const
{
	if( completed_ == state )
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.completed_ = state;
		newTask.lastChanged_ = time(nullptr);
		return newTask;
	}
}

Task Task::withDeleted(bool state) const
{
	if( deleted_ == state )
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.deleted_ = state;
		newTask.lastChanged_ = time(nullptr);
		return newTask;
	}
}

bool Task::deleted() const
{
	return deleted_;
}

bool operator==(const Task &op1, const Task &op2)
{
	return (op1.name_ == op2.name_ &&
			op1.ID_ == op2.ID_ &&
			op1.parentID_ == op2.parentID_ &&
			op1.uuid_ == op2.uuid_ &&
			op1.parentUuid_ == op2.parentUuid_ &&
			op1.completed_ == op2.completed_ &&
			op1.deleted_ == op2.deleted_ &&
			op1.lastChanged_ == op2.lastChanged_);
}

bool operator!=(const Task &op1, const Task &op2)
{
	return !(op1 == op2);
}

}
