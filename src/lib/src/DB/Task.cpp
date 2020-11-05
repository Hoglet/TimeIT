#include "Task.h"
#include <exception>
#include <stdexcept>

namespace DB
{

Task::Task()
{
	lastChanged = time(0);
}

Task::~Task()
{
}

std::string Task::getName() const
{
	return name;
}

Task Task::withName(std::string newName) const
{
	Task newTask(*this);
	newTask.name = newName;
	return newTask;
}

int64_t Task::getID() const
{
	return ID;
}

int64_t Task::getParentID() const
{
	return parentID;
}

Task Task::withParent(int64_t op1) const
{
	Task newTask(*this);
	newTask.parentID = op1;
	return newTask;
}

Task::Task(const std::string &op_name, int64_t op_parentID)
{
	init(op_name, op_parentID, "", false, 0, 0, "", false);
}

Task::Task(const std::string &op_name, int64_t op_parentID, const std::string &op_uuid, bool op_completed, int64_t op_ID, time_t op_lastChange,
		const std::string &op_parentUUID, bool op_deleted)
{
	init(op_name, op_parentID, op_uuid, op_completed, op_ID, op_lastChange, op_parentUUID, op_deleted);
}

void Task::init(const std::string &op_name, int64_t op_parentID, const std::string &op_uuid, bool op_completed, int64_t op_ID, time_t op_lastChange,
		const std::string &op_parentUUID, bool op_deleted)
{
	if (op_uuid.length() != 0 && uuidManager.isValid(op_uuid) == false)
	{
		throw std::invalid_argument("Invalid uuid");
	}
	if (op_parentUUID.length() != 0 && uuidManager.isValid(op_parentUUID) == false)
	{
		throw std::invalid_argument("Invalid uuid in parent");
	}
	if (op_lastChange == 0)
	{
		lastChanged = time(0);
	}
	else
	{
		lastChanged = op_lastChange;
	}
	ID = op_ID;
	parentID = op_parentID;
	name = op_name;
	uuid = op_uuid;
	completed = op_completed;
	parentUuid = op_parentUUID;
	deleted = op_deleted;
}

std::string Task::getUUID() const
{
	return uuid;
}

std::string Task::getParentUUID() const
{
	return parentUuid;
}

bool Task::getCompleted() const
{
	return completed;
}
time_t Task::getLastChanged() const
{
	return lastChanged;
}

Task Task::withCompleted(bool state) const
{
	Task newTask(*this);
	newTask.completed = state;
	return newTask;
}

Task Task::withDeleted(bool state) const
{
	Task newTask(*this);
	newTask.deleted = state;
	return newTask;
}

bool Task::getDeleted() const
{
	return deleted;
}

bool operator==(const Task &op1, const Task &op2)
{
	return (op1.name == op2.name && op1.ID == op2.ID && op1.parentID == op2.parentID && op1.uuid == op2.uuid && op1.parentUuid == op2.parentUuid
			&& op1.completed == op2.completed && op1.deleted == op2.deleted && op1.lastChanged == op2.lastChanged);
}

bool operator!=(const Task &op1, const Task &op2)
{
	return !(op1 == op2);
}

}
