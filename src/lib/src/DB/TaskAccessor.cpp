#include <sstream>
#include <libtimeit/db/TaskAccessor.h>

namespace libtimeit
{

using namespace std;

TaskAccessor::TaskAccessor(	Database& op_database )
		:
		database(op_database)
{
}

TaskAccessor::~TaskAccessor()
{
}


void TaskAccessor::createTable()
{
	database.exe("CREATE TABLE IF NOT EXISTS tasks "
			" (id         INTEGER PRIMARY KEY,"
			" name        VARCHAR,"
			" parent      INTEGER,"
			" changed     INTEGER,"
			" uuid        TEXT UNIQUE,"
			" expanded    BOOL DEFAULT 0,"
			" deleted     BOOL DEFAULT 0,"
			" completed   BOOL DEFAULT 0,"
			" FOREIGN KEY(parent) REFERENCES tasks(id) "
			")");
}

void TaskAccessor::enableNotifications(bool state)
{
	database.enableNotifications(state);
}

vector<Task> TaskAccessor::getTasks(int64_t parentID)
{
	vector<Task> retVal;
	stringstream statement;

	statement << "SELECT id, parent, name, completed, uuid, changed"
			"  FROM "
			"    tasks"
			"  WHERE deleted=0";
	if (parentID > 0)
	{
		statement << " AND parent=" << parentID;
	}
	else
	{
		statement << " AND parent IS NULL ";
	}

	QueryResult rows = database.exe(statement.str());
	for (std::vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		int parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		auto uuid = toUuid(row[4].getString());
		time_t lastChange = row[5].getInt();

		if(uuid)
		{
			Task task(name, parent, *uuid, completed, id, lastChange, idToUuid(parent), false);
			retVal.push_back(task);
		}
	}
	return retVal;
}

optional<Task> TaskAccessor::getTask(int64_t taskID)
{
	Statement statement_getTask = database.prepare("SELECT id, parent, name, completed, uuid, changed, deleted FROM  tasks WHERE id=?");
	statement_getTask.bindValue(1, taskID);
	QueryResult rows = statement_getTask.execute();
	shared_ptr<Task> task;
	if (rows.size() == 1)
	{
		std::vector<DataCell> row = rows.at(0);
		int64_t id = row[0].getInt();
		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		auto uuid = toUuid(row[4].getString());
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		if(uuid)
		{
			auto parentUUID = idToUuid(parent);
			Task task(name, parent, *uuid, completed, id, lastChange, parentUUID, deleted);
			return { task };
		}
	}
	return {};
}

optional<Task> TaskAccessor::getTaskUnlimited(int64_t taskID)
{
	Statement statement_getCompleteTask = database.prepare("SELECT id, parent, name, completed,"
			" uuid, changed, deleted FROM  tasks WHERE id=?;");
	statement_getCompleteTask.bindValue(1, taskID);
	QueryResult rows = statement_getCompleteTask.execute();
	Task task;
	if (rows.size() == 1)
	{
		std::vector<DataCell> row = rows.at(0);
		int64_t id = row[0].getInt();
		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		auto uuid = toUuid(row[4].getString());
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		if(uuid)
		{
			return {Task(name, parent, *uuid, completed, id, lastChange, idToUuid(parent), deleted)};
		}
	}
	return {};
}

vector<Task> TaskAccessor::getTasksChangedSince(time_t timestamp)
{
	vector<Task> retVal;
	stringstream statement;

	statement << "SELECT id, parent, name, completed, uuid, changed, deleted"
			"  FROM "
			"    tasks"
			"  WHERE changed >" << timestamp;

	QueryResult rows = database.exe(statement.str());
	for (std::vector<DataCell> row : rows)
	{
		int id = row[0].getInt();
		int parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		auto uuid = toUuid(row[4].getString());
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		if (uuid)
		{
			auto parentUUID=idToUuid(parent);
			Task task(name, parent, *uuid, completed, id, lastChange, parentUUID, deleted);
			retVal.push_back(task);
		}
	}
	return retVal;
}

int64_t TaskAccessor::uuidToId(UUID uuid)
{

	int64_t id = 0;
	Statement statement_uuidToId = database.prepare("SELECT id FROM tasks WHERE uuid=?;");
	statement_uuidToId.bindValue(1, uuid.c_str());
	QueryResult rows = statement_uuidToId.execute();
	for (std::vector<DataCell> row : rows)
	{
		id = row[0].getInt();
	}
	return id;
}

optional<class UUID> TaskAccessor::idToUuid(int64_t id)
{
	if(id)
	{
		Statement statement_idToUuid = database.prepare("SELECT uuid FROM tasks WHERE id=?;");
		statement_idToUuid.bindValue(1, id);
		QueryResult rows = statement_idToUuid.execute();
		for (vector<DataCell> row : rows)
		{
			auto uuid = row[0].getString();
			return toUuid(uuid);
		}
	}
	return {};
}

void TaskAccessor::_update(const Task &task)
{
	Statement statement_updateTask = database.prepare(
			"UPDATE tasks SET name = ?, parent = ? ,changed = ? ,deleted = ?, completed = ? WHERE id=?;");
	statement_updateTask.bindValue(1, task.name());
	if (task.parentID() > 0)
	{
		statement_updateTask.bindValue(2, task.parentID());
	}
	else
	{
		statement_updateTask.bindNullValue(2);
	}
	statement_updateTask.bindValue(3, task.lastChanged());
	statement_updateTask.bindValue(4, task.deleted());
	statement_updateTask.bindValue(5, task.completed());
	statement_updateTask.bindValue(6, task.ID());

	statement_updateTask.execute();
}

void TaskAccessor::notify(const Task &existingTask, const Task &task)
{
	bool parentChanged = (existingTask.parentID() != task.parentID());
	bool taskDeleted = (existingTask.deleted() != task.deleted() && task.deleted() == true);
	bool nameChanged = (existingTask.name() != task.name());

	if (nameChanged)
	{
		database.sendNotification(TASK_NAME_CHANGED, task.ID());
	}
	if (parentChanged)
	{
		database.sendNotification(TASK_PARENT_CHANGED, task.ID());
	}
	if (taskDeleted)
	{
		database.sendNotification(TASK_REMOVED, task.ID());
	}
}

bool TaskAccessor::updateTask(const Task &task)
{
	int id = task.ID();
	if (id == 0)
	{
		id = uuidToId(task.getUUID());
		if (id == 0)
		{
			stringstream message;
			message << "Unable to find task with UUID=" << task.getUUID().c_str();
			dbe.setMessage(message.str());
			throw dbe;
		}
	}
	auto existingTask = getTaskUnlimited(id);
	if (existingTask.has_value() &&  task != *existingTask && task.lastChanged() >= existingTask->lastChanged())
	{

		_update(task);
		notify(*existingTask, task);
		return true;
	}
	return false;
}

int64_t TaskAccessor::newTask(const Task &op_task)
{
	UUID uuid = op_task.getUUID();
	int parentID = op_task.parentID();
	std::string name = op_task.name();
	bool completed = op_task.completed();
	time_t changeTime = op_task.lastChanged();
	bool deleted = op_task.deleted();

	int64_t id = 0;

	Statement statement_newTask = database.prepare("INSERT INTO tasks (name,parent,changed,uuid,completed,deleted) "
			"VALUES (?,?,?,?,?,?);");
	statement_newTask.bindValue(1, name);
	if (parentID > 0)
	{
		statement_newTask.bindValue(2, parentID);
	}
	else
	{
		statement_newTask.bindNullValue(2);
	}
	statement_newTask.bindValue(3, changeTime);
	statement_newTask.bindValue(4, uuid.c_str());
	statement_newTask.bindValue(5, completed);
	statement_newTask.bindValue(6, deleted);

	statement_newTask.execute();
	id = database.getIDOfLastInsert();
	database.sendNotification(TASK_ADDED, id);
	return id;
}

int64_t TaskAccessor::newTask(std::string name, int64_t parentID)
{
	Task task(name, parentID);
	return newTask(task);
}

void TaskAccessor::setParentID(int64_t taskID, int parentID)
{
	time_t now = time(0);
	stringstream statement;
	if (parentID == 0)
	{
		statement << "UPDATE tasks SET parent = NULL ";
	}
	else
	{
		statement << "UPDATE tasks SET parent = " << parentID;
	}
	statement << " , changed = " << now;
	statement << " WHERE id=" << taskID;
	database.exe(statement.str());
	database.sendNotification(TASK_PARENT_CHANGED, taskID);
}

void TaskAccessor::removeTask(int64_t taskID)
{
	time_t now = time(0);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " ,changed = " << now;
	statement << " WHERE id = " << taskID;
	database.exe(statement.str());
	database.sendNotification(TASK_REMOVED, taskID);
}

void TaskAccessor::attach(EventObserver *observer)
{
	database.attach(observer);
}
void TaskAccessor::detach(EventObserver *observer)
{
	database.detach(observer);
}

void TaskAccessor::upgradeToDB5()
{
	time_t now = time(nullptr);
//Update Tasks to new design
	database.exe("ALTER TABLE tasks RENAME TO tasks_backup");
	createTable();
	database.exe("UPDATE tasks_backup SET deleted = 0 WHERE deleted != 1");
	database.exe("UPDATE tasks_backup SET parent = NULL WHERE parent = 0");
	Statement statement = database.prepare("SELECT id, parent, name, deleted FROM  tasks_backup");
	QueryResult rows = statement.execute();
	for (vector<DataCell> row : rows)
	{
		int64_t id = row[0].getInt();
		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool deleted = row[3].getBool();
		bool completed = false;
		time_t changed = now;

		Task task(name, parent, UUID(), completed, id, changed, {}, deleted);
		newTask(task);
	}
	database.exe("DROP TABLE tasks_backup");

}

void TaskAccessor::setTaskExpanded(int64_t taskID, bool expanded)
{
	stringstream statement;
	statement << "UPDATE tasks SET expanded = " << expanded;
	statement << " WHERE id=" << taskID;
	database.exe(statement.str());
}

}
