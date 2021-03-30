
#include <stdexcept>
#include <libtimeit/db/task.h>

namespace libtimeit
{

Task::Task()
{
	last_changed_ = time(nullptr);
}

Task::~Task()
{
}

string Task::name() const
{
	return name_;
}

Task Task::with_name( string new_name) const
{
	if(name_ == new_name)
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.name_ = new_name;
		newTask.last_changed_ = time(nullptr);
		return newTask;
	}
}

Task_ID Task::ID() const
{
	return ID_;
}

Task_ID Task::parent_ID() const
{
	return parent_ID_;
}

Task Task::with_parent(Task_ID parent_ID) const
{
	if(parent_ID_ == parent_ID)
	{
		return *this;
	}
	else
	{
		Task new_task(*this);

		new_task.parent_ID_ = parent_ID;
		new_task.last_changed_ = time(nullptr);
		return new_task;
	}
}

Task::Task( string op_name, Task_ID parent_ID): Task(op_name, parent_ID, UUID(), false, 0, 0, {}, false)
{

}

Task::Task(
	string              op_name,
	Task_ID              op_parentID,
	class UUID           op_uuid,
	bool                 op_completed,
	Task_ID              op_ID,
	time_t               op_lastChange,
	optional<class UUID> op_parentUUID,
	bool                 op_deleted)
	:
		uuid_(op_uuid),
		parent_Uuid_(op_parentUUID)
{
	if (op_lastChange == 0)
	{
		last_changed_ = time(nullptr);
	}
	else
	{
		last_changed_ = op_lastChange;
	}
	ID_ = op_ID;
	parent_ID_ = op_parentID;
	name_ = op_name;
	completed_ = op_completed;
	deleted_ = op_deleted;
}

UUID Task::get_UUID() const
{
	return uuid_;
}

optional<class UUID> Task::parent_UUID() const
{
	return parent_Uuid_;
}

bool Task::completed() const
{
	return completed_;
}
time_t Task::last_changed() const
{
	return last_changed_;
}

Task Task::with_completed(bool state) const
{
	if( completed_ == state )
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.completed_ = state;
		newTask.last_changed_ = time(nullptr);
		return newTask;
	}
}

Task Task::with_deleted(bool state) const
{
	if( deleted_ == state )
	{
		return *this;
	}
	else
	{
		Task newTask(*this);
		newTask.deleted_ = state;
		newTask.last_changed_ = time(nullptr);
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
			op1.parent_ID_ == op2.parent_ID_ &&
			op1.uuid_ == op2.uuid_ &&
			op1.parent_Uuid_ == op2.parent_Uuid_ &&
			op1.completed_ == op2.completed_ &&
			op1.deleted_ == op2.deleted_ &&
			op1.last_changed_ == op2.last_changed_);
}

bool operator!=(const Task &op1, const Task &op2)
{
	return !(op1 == op2);
}

}
