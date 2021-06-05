#include <sstream>
#include <fmt/core.h>
#include <libtimeit/db/task_accessor.h>

namespace libtimeit
{

using namespace std;

const string create_task_query = R"Query(
				INSERT INTO
					tasks (name,parent,changed,uuid,completed,deleted,idle,quiet)
				VALUES (?,?,?,?,?,?,?,?);
				)Query";



Task_accessor::Task_accessor(	Database& op_database )
		:
		database(op_database),
		statement_uuid_to_id(database.prepare("SELECT id FROM tasks WHERE uuid=?;")),
		statement_get_task(database.prepare(R"(
				SELECT
					id,
					parent,
					name,
					completed,
					uuid,
					changed,
					deleted,
					idle,
					quiet
				FROM
					tasks
				WHERE
					id=?)")),
		statement_id_to_uuid(database.prepare("SELECT uuid FROM tasks WHERE id=?;")),
		statement_new_task(database.prepare( create_task_query ))
{
}



void Task_accessor::create_table(Database& db)
{
	Statement create_statement = db.prepare( R"Query(
		CREATE TABLE IF NOT EXISTS
 			tasks
			 (
				id         INTEGER PRIMARY KEY,
				name        VARCHAR,
				parent      INTEGER,
				changed     INTEGER,
				uuid        TEXT UNIQUE,
				expanded    BOOL DEFAULT    0,
				deleted     BOOL DEFAULT    0,
			 	completed   BOOL DEFAULT    0,
				idle        INTEGER DEFAULT 0,
				quiet       BOOL DEFAULT    0,
			 FOREIGN KEY(parent) REFERENCES tasks(id)
			)
			)Query"
			);
	create_statement.execute();
}

void Task_accessor::enable_notifications(bool state)
{
	database.enable_notifications(state);
}

vector<Task> Task_accessor::by_parent_id(int64_t parent)
{
	vector<Task> return_value;
	stringstream statement;

	statement << "SELECT id, parent, name, completed, uuid, changed, idle, quiet"
			"  FROM "
			"    tasks"
			"  WHERE deleted=0";
	if (parent > 0)
	{
		statement << " AND parent=" << parent;
	}
	else
	{
		statement << " AND parent IS NULL ";
	}

	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
	{
		auto id = row[0].integer();
		parent = 0;
		if (row[1].has_value())
		{
			parent = row[1].integer();
		}
		auto column{2};
		string name        = row[column++].text();
		bool   completed   = row[column++].boolean();
		auto   l_uuid      = UUID::from_string(row[column++].text());
		time_t last_change = row[column++].integer();
		auto   idle        = (unsigned)row[column++].integer();
		bool   quiet       = row[column].boolean();
		if(l_uuid.has_value())
		{
			Task task(name, parent, *l_uuid, completed, id, last_change, uuid(parent), false, idle, quiet);
			return_value.push_back(task);
		}
	}
	return return_value;
}

optional<Task> Task_accessor::by_id(int64_t taskID)
{
	statement_get_task.bind_value(1, taskID);
	Query_result rows = statement_get_task.execute();
	shared_ptr<Task> task;
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		int64_t id = row[0].integer();
		int64_t parent = 0;
		if (row[1].has_value())
		{
			parent = row[1].integer();
		}
		auto column{2};
		string name        = row[column++].text();
		bool   completed   = row[column++].boolean();
		auto   l_uuid      = UUID::from_string(row[column++].text());
		time_t last_change = row[column++].integer();
		bool   deleted     = row[column++].boolean();
		auto   idle        = (unsigned)row[column++].integer();
		bool   quiet       = row[column].boolean();
		if(l_uuid.has_value())
		{
			auto parent_uuid = uuid(parent);
			return Task(name, parent, *l_uuid, completed, id, last_change, parent_uuid, deleted, idle, quiet);
		}
	}
	return {};
}

optional<Task> Task_accessor::get_task_unlimited(Task_id taskID)
{
	Statement statement_get_complete_task = database.prepare(R"(
		SELECT
			id,
			parent,
			name,
			completed,
			uuid,
			changed,
			deleted,
			idle,
			quiet
		FROM
			tasks
		WHERE
			id=?;
		)");
	statement_get_complete_task.bind_value(1, taskID);
	Query_result rows = statement_get_complete_task.execute();
	if (rows.size() == 1)
	{
		vector<Data_cell> row = rows.at(0);
		int64_t id = row[0].integer();
		int64_t parent = 0;
		if (row[1].has_value())
		{
			parent = row[1].integer();
		}
		auto column{2};
		string   name        = row[column++].text();
		bool     completed   = row[column++].boolean();
		auto     l_uuid      = UUID::from_string(row[column++].text());
		time_t   last_change = row[column++].integer();
		bool     deleted     = row[column++].boolean();
		auto     idle        = (unsigned)row[column++].integer();
		bool     quiet       = row[column].boolean();
		if(l_uuid.has_value())
		{
			return {Task(name, parent, *l_uuid, completed, id, last_change, uuid(parent), deleted, idle, quiet)};
		}
	}
	return {};
}

vector<Task> Task_accessor::changed_since(time_t timestamp)
{
	vector<Task> return_value;
	stringstream statement;

	statement << R"(
			SELECT
				id,
				parent,
				name,
				completed,
				uuid,
				changed,
				deleted,
				idle,
				quiet
			FROM
			    tasks
			WHERE
				changed > )"
				<< timestamp;

	Query_result rows = database.execute(statement.str());
	for (std::vector<Data_cell> row : rows)
	{
		auto column{0};
		auto id = (Task_id)row[column++].integer();
		Task_id parent = 0;
		if (row[column].has_value())
		{
			parent = (Task_id)row[column].integer();
		}
		column++;
		string  name        = row[column++].text();
		bool    completed   = row[column++].boolean();
		auto    l_uuid      = UUID::from_string(row[column++].text());
		time_t  last_change = row[column++].integer();
		bool    deleted     = row[column++].boolean();
		auto    idle        = (unsigned)row[column++].integer();
		bool    quiet       = row[column].boolean();
		if (l_uuid.has_value())
		{
			auto parent_uuid= uuid(parent);
			Task task(name, parent, *l_uuid, completed, id, last_change, parent_uuid, deleted, idle, quiet);
			return_value.push_back(task);
		}
	}
	return return_value;
}

Task_id Task_accessor::id(UUID uuid)
{

	Task_id id = 0;
	statement_uuid_to_id.bind_value(1, uuid.to_string());
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<Data_cell> row : rows)
	{
		id = row[0].integer();
	}
	return id;
}

optional<class UUID> Task_accessor::uuid(Task_id id)
{
	if( id>0 )
	{
		statement_id_to_uuid.bind_value(1, id);
		Query_result rows = statement_id_to_uuid.execute();
		for (vector<Data_cell> row : rows)
		{
			auto uuid = row[0].text();
			return UUID::from_string(uuid);
		}
	}
	return {};
}

void Task_accessor::internal_update(const Task &task)
{
	Statement statement_update_task = database.prepare(
			R"(
				UPDATE
					tasks
				SET
					name = ?,
					parent = ? ,
					changed = ? ,
					deleted = ?,
					completed = ?,
					idle = ?,
					quiet = ?
				WHERE id=?;)"
						);
	auto index{1};
	statement_update_task.bind_value(index++, task.name);
	if (task.parent_id > 0)
	{
		statement_update_task.bind_value(index++, task.parent_id);
	}
	else
	{
		statement_update_task.bind_null_value(index++);
	}
	statement_update_task.bind_value(index++, task.last_changed);
	statement_update_task.bind_value(index++, (int64_t)task.deleted);
	statement_update_task.bind_value(index++, (int64_t)task.completed);
	statement_update_task.bind_value(index++, task.idle);
	statement_update_task.bind_value(index++, (int64_t)task.quiet);
	statement_update_task.bind_value(index,   task.id);

	statement_update_task.execute();
}

void Task_accessor::notify(const Task &existingTask, const Task &task)
{
	bool parent_changed = (existingTask.parent_id != task.parent_id);
	bool task_deleted   = ((existingTask.deleted   != task.deleted) && task.deleted);
	bool name_changed   = (existingTask.name      != task.name);

	if (name_changed)
	{
		database.send_notification(TASK_NAME_CHANGED, task.id);
	}
	if (parent_changed)
	{
		database.send_notification(TASK_PARENT_CHANGED, task.id);
	}
	if (task_deleted)
	{
		database.send_notification(TASK_REMOVED, task.id);
	}
}

bool Task_accessor::update(const Task &task)
{
	if (task.id == 0)
	{
		if (id(task.uuid) == 0)
		{
			stringstream message;
			message << "Unable to find task with UUID=" << task.uuid.c_str();
			throw db_exception(message.str());
		}
	}
	auto existing_task = get_task_unlimited(task.id);
	if (existing_task.has_value() && task != *existing_task && task.last_changed >= existing_task->last_changed)
	{
		internal_update(task);
		notify(*existing_task, task);
		return true;
	}
	return false;
}

Task_id Task_accessor::create(const Task &task)
{

	internal_create(task, statement_new_task);
	Task_id id = database.id_of_last_insert();

	database.send_notification(TASK_ADDED, id);
	return id;
}

void Task_accessor::internal_create(const Task &task, Statement &statement_new_task)
{
	auto index{1};
	statement_new_task.bind_value(index++, task.name);
	if (task.parent_id > 0)
	{
		statement_new_task.bind_value(index++, task.parent_id);
	}
	else
	{
		statement_new_task.bind_null_value(index++);
	}
	statement_new_task.bind_value(index++, task.last_changed);
	statement_new_task.bind_value(index++, task.uuid.c_str());
	statement_new_task.bind_value(index++, (int64_t)task.completed);
	statement_new_task.bind_value(index++, (int64_t)task.deleted);
	statement_new_task.bind_value(index++,          task.idle);
	statement_new_task.bind_value(index,   (int64_t)task.quiet);

	statement_new_task.execute();
}

void Task_accessor::set_parent_id(Task_id taskID, Task_id parentID)
{
	time_t now = time(nullptr);
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
	database.execute(statement.str());
	database.send_notification(TASK_PARENT_CHANGED, taskID);
}

void Task_accessor::remove(int64_t taskID)
{
	time_t now = time(nullptr);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " ,changed = " << now;
	statement << " WHERE id = " << taskID;
	database.execute(statement.str());
	database.send_notification(TASK_REMOVED, taskID);
}

void Task_accessor::upgrade_to_db_5(Database& database)
{
	time_t now = time(nullptr);
//Update Tasks to new design
	database.execute("ALTER TABLE tasks RENAME TO tasks_backup");
	Task_accessor::create_table(database);
	auto statement_new_task(database.prepare( create_task_query ));

	database.execute("UPDATE tasks_backup SET deleted = 0 WHERE deleted != 1");
	database.execute("UPDATE tasks_backup SET parent = NULL WHERE parent = 0");
	Statement statement = database.prepare("SELECT id, parent, name, deleted FROM  tasks_backup");
	Query_result rows = statement.execute();
	for (vector<Data_cell> row : rows)
	{
		int64_t id = row[0].integer();
		int64_t parent = 0;
		if (row[1].has_value())
		{
			parent = row[1].integer();
		}
		string name      = row[2].text();
		bool   deleted   = row[3].boolean();
		bool   completed = false;
		time_t changed   = now;

		Task task(name, parent, UUID(), completed, id, changed, {}, deleted, 0, false);
		internal_create(task, statement_new_task);
	}
	database.execute("DROP TABLE tasks_backup");

}

void Task_accessor::upgrade(Database& database)
{
	if (database.current_db_version() < 5 ) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		upgrade_to_db_5(database);
	}
	if( ! database.column_exists("tasks", "idle") )
	{
		const auto *query = R"Query(
			ALTER TABLE
				tasks
			ADD
				idle        INTEGER
			DEFAULT 0
				;
			)Query";
		database.execute( query );

		const auto *query2 = R"Query(
			ALTER TABLE
				tasks
			ADD
				quiet      BOOL
			DEFAULT 0
				;
			)Query";
		database.execute( query2 );
	}
}



void Task_accessor::set_task_expanded(Task_id taskID, bool expanded)
{
	auto statement = fmt::format(
			R"Query(
				UPDATE
					tasks
				SET
					expanded = {}
				WHERE
					id= {} ;
			)Query"
					, expanded, taskID);
	database.execute(statement);
}

void Task_accessor::setup(Database& database)
{
	Task_accessor::create_table(database);
	Task_accessor::upgrade(database);
}

}

