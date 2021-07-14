#include <sstream>
#include <fmt/core.h>
#include <libtimeit/db/task_accessor.h>

namespace libtimeit
{

using namespace std;

const string CREATE_TASK_QUERY = R"Query(
				INSERT INTO
					tasks (name,parent,changed,uuid,completed,deleted,idle,quiet)
				VALUES (?,?,?,?,?,?,?,?);
				)Query";



task_accessor::task_accessor(database& op_database )
		:
		db(op_database),
		statement_uuid_to_id(db.prepare("SELECT id FROM tasks WHERE uuid=?;")),
		statement_get_task(db.prepare(R"(
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
		statement_id_to_uuid(db.prepare("SELECT uuid FROM tasks WHERE id=?;")),
		statement_new_task(db.prepare(CREATE_TASK_QUERY ))
{
}



void task_accessor::create_table(database& db)
{
	sql_statement create_statement = db.prepare(R"Query(
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

void task_accessor::enable_notifications(bool state)
{
	db.enable_notifications(state);
}

vector<task> task_accessor::by_parent_id(int64_t parent)
{
	vector<task> return_value;
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

	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
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
			return_value.emplace_back(
					name,
					parent,
					*l_uuid,
					completed,
					id,
					last_change,
					uuid(parent),
					false,
					idle,
					quiet);
		}
	}
	return return_value;
}

optional<task> task_accessor::by_id(int64_t taskID)
{
	statement_get_task.bind_value(1, taskID);
	Query_result rows = statement_get_task.execute();
	if (rows.size() == 1)
	{
		vector<data_cell> row = rows.at(0);
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
			return task(name, parent, *l_uuid, completed, id, last_change, parent_uuid, deleted, idle, quiet);
		}
	}
	return {};
}

optional<task> task_accessor::get_task_unlimited(Task_id taskID)
{
	sql_statement statement_get_complete_task = db.prepare(R"(
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
		vector<data_cell> row = rows.at(0);
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
			return {task(name, parent, *l_uuid, completed, id, last_change, uuid(parent), deleted, idle, quiet)};
		}
	}
	return {};
}

vector<task> task_accessor::changed_since(time_t timestamp)
{
	vector<task> return_value;
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

	Query_result rows = db.execute(statement.str());
	for (std::vector<data_cell> row : rows)
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
			return_value.emplace_back(
					name,
					parent,
					*l_uuid,
					completed,
					id,
					last_change,
					parent_uuid,
					deleted,
					idle,
					quiet);
		}
	}
	return return_value;
}

Task_id task_accessor::id(UUID uuid)
{

	Task_id id = 0;
	statement_uuid_to_id.bind_value(1, uuid.to_string());
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<data_cell> row : rows)
	{
		id = row[0].integer();
	}
	return id;
}

optional<class UUID> task_accessor::uuid(Task_id id)
{
	if( id>0 )
	{
		statement_id_to_uuid.bind_value(1, id);
		Query_result rows = statement_id_to_uuid.execute();
		for (vector<data_cell> row : rows)
		{
			auto uuid = row[0].text();
			return UUID::from_string(uuid);
		}
	}
	return {};
}

void task_accessor::internal_update(const task &item)
{
	sql_statement statement_update_task = db.prepare(
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
	statement_update_task.bind_value(index++, item.name);
	if (item.parent_id > 0)
	{
		statement_update_task.bind_value(index++, item.parent_id);
	}
	else
	{
		statement_update_task.bind_null_value(index++);
	}
	statement_update_task.bind_value(index++, item.last_changed);
	statement_update_task.bind_value(index++, (int64_t)item.deleted);
	statement_update_task.bind_value(index++, (int64_t)item.completed);
	statement_update_task.bind_value(index++, item.idle);
	statement_update_task.bind_value(index++, (int64_t)item.quiet);
	statement_update_task.bind_value(index, item.id);

	statement_update_task.execute();
}

void task_accessor::notify(const task &existingTask, const task &item)
{
	bool parent_changed = (existingTask.parent_id != item.parent_id);
	bool task_deleted   = ((existingTask.deleted != item.deleted) && item.deleted);
	bool name_changed   = (existingTask.name != item.name);

	if (name_changed)
	{
		db.send_notification(TASK_NAME_CHANGED, item.id);
	}
	if (parent_changed)
	{
		db.send_notification(TASK_PARENT_CHANGED, item.id);
	}
	if (task_deleted)
	{
		db.send_notification(TASK_REMOVED, item.id);
	}
}

bool task_accessor::update(const task &item)
{
	if (item.id == 0)
	{
		if (id(item.uuid) == 0)
		{
			stringstream message;
			message << "Unable to find task with UUID=" << item.uuid.c_str();
			throw db_exception(message.str());
		}
	}
	auto existing_task = get_task_unlimited(item.id);
	if (existing_task.has_value() && item != *existing_task && item.last_changed >= existing_task->last_changed)
	{
		internal_update(item);
		notify(*existing_task, item);
		return true;
	}
	return false;
}

Task_id task_accessor::create(const task &item)
{

	internal_create(item, statement_new_task);
	Task_id id = db.id_of_last_insert();

	db.send_notification(TASK_ADDED, id);
	return id;
}

void task_accessor::internal_create(const task &item, sql_statement &statement_new_task)
{
	auto index{1};
	statement_new_task.bind_value(index++, item.name);
	if (item.parent_id > 0)
	{
		statement_new_task.bind_value(index++, item.parent_id);
	}
	else
	{
		statement_new_task.bind_null_value(index++);
	}
	statement_new_task.bind_value(index++, item.last_changed);
	statement_new_task.bind_value(index++, item.uuid.c_str());
	statement_new_task.bind_value(index++, (int64_t)item.completed);
	statement_new_task.bind_value(index++, (int64_t)item.deleted);
	statement_new_task.bind_value(index++, item.idle);
	statement_new_task.bind_value(index,   (int64_t)item.quiet);

	statement_new_task.execute();
}

void task_accessor::set_parent_id(Task_id taskID, Task_id parentID)
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
	db.execute(statement.str());
	db.send_notification(TASK_PARENT_CHANGED, taskID);
}

void task_accessor::remove(int64_t taskID)
{
	time_t now = time(nullptr);
	stringstream statement;
	statement << "UPDATE tasks SET deleted = " << true;
	statement << " ,changed = " << now;
	statement << " WHERE id = " << taskID;
	db.execute(statement.str());
	db.send_notification(TASK_REMOVED, taskID);
}

void task_accessor::upgrade_to_db_5(database& db)
{
	time_t now = time(nullptr);
//Update Tasks to new design
	db.execute("ALTER TABLE tasks RENAME TO tasks_backup");
	task_accessor::create_table(db);
	auto statement_new_task(db.prepare(CREATE_TASK_QUERY ));

	db.execute("UPDATE tasks_backup SET deleted = 0 WHERE deleted != 1");
	db.execute("UPDATE tasks_backup SET parent = NULL WHERE parent = 0");
	sql_statement statement = db.prepare("SELECT id, parent, name, deleted FROM  tasks_backup");
	Query_result rows = statement.execute();
	for (vector<data_cell> row : rows)
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

		task item(name, parent, UUID(), completed, id, changed, {}, deleted, 0, false);
		internal_create(item, statement_new_task);
	}
	db.execute("DROP TABLE tasks_backup");

}

void task_accessor::upgrade(database& db)
{
	if (db.current_db_version() < 5 ) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		upgrade_to_db_5(db);
	}
	if( ! db.column_exists("tasks", "idle") )
	{
		const auto *query = R"Query(
			ALTER TABLE
				tasks
			ADD
				idle        INTEGER
			DEFAULT 0
				;
			)Query";
		db.execute(query );

		const auto *query2 = R"Query(
			ALTER TABLE
				tasks
			ADD
				quiet      BOOL
			DEFAULT 0
				;
			)Query";
		db.execute(query2 );
	}
}



void task_accessor::set_task_expanded(Task_id taskID, bool expanded)
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
	db.execute(statement);
}

void task_accessor::setup(database& db)
{
	task_accessor::create_table(db);
	task_accessor::upgrade(db);
}

}

