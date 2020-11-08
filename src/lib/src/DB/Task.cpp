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

Task::Task(const string &op_name, int64_t op_parentID): Task(op_name, op_parentID, "", false, 0, 0, "", false)
{

}

Task::Task(
	const std::string& op_name,
	int64_t            op_parentID,
	const std::string& op_uuid,
	bool               op_completed,
	int64_t            op_ID,
	time_t             op_lastChange,
	const std::string& op_parentUUID,
	bool               op_deleted)
{
	if (op_uuid.length() != 0 && UUIDTool::isValid(op_uuid) == false)
	{
		throw std::invalid_argument("Invalid uuid");
	}
	if (op_parentUUID.length() != 0 && UUIDTool::isValid(op_parentUUID) == false)
	{
		throw std::invalid_argument("Invalid uuid in parent");
	}
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
	uuid_ = op_uuid;
	completed_ = op_completed;
	parentUuid_ = op_parentUUID;
	deleted_ = op_deleted;
}

std::string Task::UUID() const
{
	return uuid_;
}

std::string Task::parentUUID() const
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
