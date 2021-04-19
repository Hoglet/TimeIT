
#include <stdexcept>
#include <libtimeit/db/task.h>

namespace libtimeit
{


Task Task::with_name( string new_name) const
{
	if(name == new_name)
	{
		return *this;
	}
	else
	{
		return 	Task(
				new_name,
				parent_ID,
				uuid,
				completed,
				ID,
				time(nullptr),
				parent_uuid,
				deleted,
				idle,
				quiet);

	}
}

Task Task::with_parent(Task_ID new_parent_ID) const
{
	if(parent_ID == new_parent_ID)
	{
		return *this;
	}
	else
	{
		return 	Task(
				name,
				new_parent_ID,
				uuid,
				completed,
				ID,
				time(nullptr),
				parent_uuid,
				deleted,
				idle,
				quiet);
	}
}

Task::Task(string name_, Task_ID parent_ID_)
	:
	Task(
			name_,
			parent_ID_,
			UUID(),
			false,
			0,
			time( nullptr ),
			{},
			false,
			0,
			false)
{

}

Task::Task(
	string               name_,
	Task_ID              parentID_,
	class UUID           uuid_,
	bool                 completed_,
	Task_ID              ID_,
	time_t               last_change_,
	optional<class UUID> parent_uuid_,
	bool                 deleted_,
	int                  idle_,
	bool                 quiet_)
	:
		name(name_),
		parent_ID(parentID_),
		uuid(uuid_),
		completed(completed_),
		ID(ID_),
		last_changed(last_change_),
		parent_uuid(parent_uuid_),
		deleted(deleted_),
		idle(idle_),
		quiet(quiet_)
{

}



Task Task::with_completed(bool new_completed) const
{
	if(completed == new_completed )
	{
		return *this;
	}
	else
	{
		return 	Task(
				name,
				parent_ID,
				uuid,
				new_completed,
				ID,
				time(nullptr),
				parent_uuid,
				deleted,
				idle,
				quiet);
	}
}

Task Task::with_deleted(bool new_deleted) const
{
	if(deleted == new_deleted )
	{
		return *this;
	}
	else
	{
		return 	Task(
				name,
				parent_ID,
				uuid,
				completed,
				ID,
				time(nullptr),
				parent_uuid,
				new_deleted,
				idle,
				quiet);
	}
}


bool operator==(const Task &op1, const Task &op2)
{
	return (op1.name         == op2.name &&
			op1.ID           == op2.ID &&
			op1.parent_ID    == op2.parent_ID &&
			op1.uuid         == op2.uuid &&
			op1.parent_uuid == op2.parent_uuid &&
			op1.completed    == op2.completed &&
			op1.deleted      == op2.deleted &&
			op1.last_changed == op2.last_changed);
}

bool operator!=(const Task &op1, const Task &op2)
{
	return !(op1 == op2);
}

}
