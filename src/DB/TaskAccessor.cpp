#include "TaskAccessor.h"
#include <UUID.h>
#include <sstream>

using namespace DBAbstraction;
using namespace std;
namespace DB
{

TaskAccessor::TaskAccessor(std::shared_ptr<DBAbstraction::CSQL>& op_db, std::shared_ptr<Notifier> notifier)
{
	db = op_db;
	this->notifier = notifier;
}

TaskAccessor::~TaskAccessor()
{
}

void TaskAccessor::createTable()
{
	db->exe("CREATE TABLE IF NOT EXISTS tasks "
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
	notifier->enabled(state);
}

std::shared_ptr<std::vector<Task>> TaskAccessor::getTasks(int64_t parentID)
{
	shared_ptr<vector<Task>> retVal = shared_ptr<std::vector<Task>>(new vector<Task>);
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

	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (std::vector<DataCell> row : *rows)
	{
		int id = row[0].getInt();
		int parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		string uuid = row[4].getString();
		time_t lastChange = row[5].getInt();

		Task task(name, parent, uuid, completed, id, lastChange, "", false);
		retVal->push_back(task);
	}
	return retVal;
}

std::shared_ptr<Task> TaskAccessor::getTask(int64_t taskID)
{
	std::shared_ptr<DBAbstraction::Statement> statement_getTask = db->prepare("SELECT id, parent, name, completed,"
				" uuid, changed, deleted FROM  tasks WHERE id=? AND deleted=0;");
	statement_getTask->bindValue(1, taskID);
	shared_ptr<QueryResult> rows = statement_getTask->execute();
	shared_ptr<Task> task;
	if (rows->size() == 1)
	{
		std::vector<DataCell> row = rows->at(0);
		int64_t id = row[0].getInt();
		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		string uuid = row[4].getString();
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		string parentUUID;
		if (parent != 0)
		{
			parentUUID = idToUuid(parent);
		}
		task = std::shared_ptr<Task>(new Task(name, parent, uuid, completed, id, lastChange, parentUUID, deleted));
	}
	return task;
}

std::shared_ptr<Task> TaskAccessor::getTaskUnlimited(int64_t taskID)
{
	std::shared_ptr<DBAbstraction::Statement> statement_getCompleteTask = db->prepare("SELECT id, parent, name, completed,"
				" uuid, changed, deleted FROM  tasks WHERE id=?;");
	statement_getCompleteTask->bindValue(1, taskID);
	std::shared_ptr<QueryResult> rows = statement_getCompleteTask->execute();
	std::shared_ptr<Task> task;
	if (rows->size() == 1)
	{
		std::vector<DataCell> row = rows->at(0);
		int64_t id = row[0].getInt();

		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		string uuid = row[4].getString();
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		string parentUUID;
		if (parent != 0)
		{
			parentUUID = idToUuid(parent);
		}
		task = std::shared_ptr<Task>(new Task(name, parent, uuid, completed, id, lastChange, parentUUID, deleted));
	}
	return task;
}

std::shared_ptr<std::vector<Task>> TaskAccessor::getTasksChangedSince(time_t timestamp)
{
	shared_ptr<vector<Task>> retVal = shared_ptr<std::vector<Task>>(new vector<Task>);
	stringstream statement;

	statement << "SELECT id, parent, name, completed, uuid, changed, deleted"
			"  FROM "
			"    tasks"
			"  WHERE changed >" << timestamp;

	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (std::vector<DataCell> row : *rows)
	{
		int id = row[0].getInt();
		int parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		bool completed = row[3].getBool();
		string uuid = row[4].getString();
		time_t lastChange = row[5].getInt();
		bool deleted = row[6].getBool();
		string parentUuid = "";
		Task task(name, parent, uuid, completed, id, lastChange, "", deleted);
		retVal->push_back(task);
	}

	for (unsigned int position = 0; position < retVal->size(); position++)
	{
		int64_t parent = retVal->at(position).getParentID();
		if (parent != 0)
		{
			retVal->at(position).parentUuid = idToUuid(parent);
		}
	}
	return retVal;
}

int64_t TaskAccessor::uuidToId(std::string uuid)
{

	int64_t id = 0;
	std::shared_ptr<DBAbstraction::Statement> statement_uuidToId = db->prepare("SELECT id FROM tasks WHERE uuid=?;");
	statement_uuidToId->bindValue(1, uuid);
	std::shared_ptr<QueryResult> rows = statement_uuidToId->execute();
	for (std::vector<DataCell> row : *rows)
	{
		id = row[0].getInt();
	}
	return id;
}

std::string TaskAccessor::idToUuid(int64_t id)
{

	string uuid = "";
	std::shared_ptr<DBAbstraction::Statement> statement_idToUuid = db->prepare("SELECT uuid FROM tasks WHERE id=?;");
	statement_idToUuid->bindValue(1, id);
	std::shared_ptr<QueryResult> rows = statement_idToUuid->execute();
	for (vector<DataCell> row : *rows)
	{
		uuid = row[0].getString();
	}
	return uuid;
}

bool TaskAccessor::updateTask(const Task& task)
{
	int id = task.getID();
	if (id == 0)
	{
		id = uuidToId(task.getUUID());
		if (id == 0)
		{
			stringstream message;
			message << "Unable to find task with UUID=" << task.getUUID();
			dbe.setMessage(message.str());
			throw dbe;
		}
	}
	std::shared_ptr<Task> existingTask = getTaskUnlimited(id);
	if (task != *existingTask && task.getLastChanged() >= existingTask->getLastChanged())
	{
		bool parentChanged = (existingTask->getParentID() != task.getParentID());
		bool taskDeleted = (existingTask->getDeleted() != task.getDeleted() && task.getDeleted() == true);
		std::shared_ptr<DBAbstraction::Statement> statement_updateTask = db->prepare(
					"UPDATE tasks SET name = ?, parent = ? ,changed = ? ,deleted = ?, completed = ? WHERE id=?;");
		statement_updateTask->bindValue(1, task.getName());
		if (task.getParentID() > 0)
		{
			statement_updateTask->bindValue(2, task.getParentID());
		}
		else
		{
			statement_updateTask->bindNullValue(2);
		}
		statement_updateTask->bindValue(3, task.getLastChanged());
		statement_updateTask->bindValue(4, task.getDeleted());
		statement_updateTask->bindValue(5, task.getCompleted());
		statement_updateTask->bindValue(6, task.getID());

		statement_updateTask->execute();

		notifier->sendNotification(TASK_UPDATED,id);
		if (parentChanged)
		{
			notifier->sendNotification(TASK_PARENT_CHANGED, id);
		}
		if (taskDeleted)
		{
			notifier->sendNotification(TASK_REMOVED, id);
		}
		return true;
	}
	return false;
}

int64_t TaskAccessor::newTask(const Task& op_task)
{
	std::string uuid = op_task.getUUID();
	int parentID = op_task.getParentID();
	std::string name = op_task.getName();
	bool completed = op_task.getCompleted();
	time_t changeTime = op_task.getLastChanged();
	bool deleted = op_task.getDeleted();
	stringstream statement;

	if (uuid.length() == 0)
	{
		uuid = UUID::randomUUID();
	}

	int64_t id = 0;

	std::shared_ptr<DBAbstraction::Statement> statement_newTask = db->prepare("INSERT INTO tasks (name,parent,changed,uuid,completed,deleted) "
				"VALUES (?,?,?,?,?,?);");
	statement_newTask->bindValue(1, name);
	if (parentID > 0)
	{
		statement_newTask->bindValue(2, parentID);
	}
	else
	{
		statement_newTask->bindNullValue(2);
	}
	statement_newTask->bindValue(3, changeTime);
	statement_newTask->bindValue(4, uuid);
	statement_newTask->bindValue(5, completed);
	statement_newTask->bindValue(6, deleted);

	statement_newTask->execute();
	id = db->getIDOfLastInsert();
	notifier->sendNotification(TASK_ADDED,id);
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
	if(parentID == 0)
	{
		statement << "UPDATE tasks SET parent = NULL ";
	}
	else
	{
		statement << "UPDATE tasks SET parent = " << parentID;
	}
	statement << " , changed = " << now;
	statement << " WHERE id=" << taskID;
	db->exe(statement.str());
	notifier->sendNotification(TASK_PARENT_CHANGED, taskID);
}

void TaskAccessor::removeTask(int64_t taskID)
{
	time_t now = time(0);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " ,changed = " << now;
	statement << " WHERE id = " << taskID;
	db->exe(statement.str());
	notifier->sendNotification(TASK_REMOVED, taskID);
}

void TaskAccessor::attach(TaskAccessorObserver* observer)
{
	notifier->attach(observer);
}
void TaskAccessor::detach(TaskAccessorObserver* observer)
{
	notifier->detach(observer);
}

void TaskAccessor::upgradeToDB5()
{
	time_t now = time(0);
	//Update Tasks to new design
	db->exe("ALTER TABLE tasks RENAME TO tasks_backup");
	createTable();
	db->exe("UPDATE tasks_backup SET deleted = 0 WHERE deleted != 1");
	db->exe("UPDATE tasks_backup SET parent = NULL WHERE parent = 0");
	shared_ptr<Statement> statement = db->prepare("SELECT id, parent, name, deleted FROM  tasks_backup");
	shared_ptr<QueryResult> rows = statement->execute();
	for (vector<DataCell> row : *rows)
	{
		int64_t id = row[0].getInt();
		int64_t parent = 0;
		if (row[1].hasValue())
		{
			parent = row[1].getInt();
		}
		string name = row[2].getString();
		string uuid = UUID::randomUUID();
		bool deleted = row[3].getBool();
		bool completed = false;
		time_t changed = now;
		string parentUUID = "";

		Task task(name, parent, uuid, completed, id, changed, parentUUID, deleted);
		newTask(task);
	}
	db->exe("DROP TABLE tasks_backup");

}

}
