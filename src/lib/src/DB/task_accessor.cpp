#include <sstream>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/utils.h>


namespace libtimeit
{

using namespace std;
using namespace std::chrono;

const string CREATE_TASK_V6 = R"Query(
				INSERT INTO
					tasks (name,parent,changed,uuid, deleted,idle,quiet)
				VALUES (?,?,?,?,?,?,?);
				)Query";

const string GET_TASK_QUERY = R"(
				SELECT
					parent,
					name,
					changed,
					deleted,
					idle,
					quiet
				FROM
					tasks
				WHERE
					uuid=?)";


task_accessor::task_accessor(database& op_database )
		:
		db(op_database),
		statement_uuid_to_id(db.prepare("SELECT id FROM tasks WHERE uuid=?;")),
		statement_get_by_id(db.prepare(GET_TASK_QUERY)),
		statement_get_task(db.prepare(R"(
				SELECT
					parent,
					name,
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
		statement_new_task(db.prepare(CREATE_TASK_V6 ))
{
}



void task_accessor::create_table(database& db)
{
	sql_statement create_statement = db.prepare(R"Query(
		CREATE TABLE IF NOT EXISTS
 			tasks
			 (
				id          INTEGER PRIMARY KEY,
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

vector<task> task_accessor::by_parent_id(optional<task_id> parent)
{
	vector<task> return_value;
	stringstream statement;

	statement << "SELECT name, uuid, changed, idle, quiet"
			"  FROM "
			"    tasks"
			"  WHERE deleted=0";
	if (parent.has_value())
	{
		auto parent_id( to_id (parent.value()));
		statement << " AND parent=" << parent_id;
	}
	else
	{
		statement << " AND parent IS NULL ";
	}

	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
	{
		auto column{0};
		string name        = row[column++].text();
		auto   l_uuid      = libtimeit::optional_task_id(row[column++].text());
		auto   last_change = system_clock::from_time_t(row[column++].integer());
		auto   idle        = minutes(row[column++].integer());
		bool   quiet       = row[column].boolean();
		if(l_uuid.has_value())
		{
			return_value.emplace_back(
					name,
					*l_uuid,
					last_change,
					parent,
					false,
					idle,
					quiet);
		}
	}
	return return_value;
}

optional<task> task_accessor::by_id(const task_id& id)
{
	statement_get_by_id.bind_value(1, static_cast<string>(id));
	Query_result rows = statement_get_by_id.execute();
	if (rows.size() == 1)
	{
		vector<data_cell> row = rows.at(0);
		int64_t parent = 0;
		if (row[0].has_value())
		{
			parent = row[0].integer();
		}
		auto column{1};
		string name        = row[column++].text();
		auto   last_change = system_clock::from_time_t( row[column++].integer());
		bool   deleted     = row[column++].boolean();
		auto   idle        = minutes(row[column++].integer());
		bool   quiet       = row[column].boolean();
		auto   parent_uuid = to_task_id(parent);

		return task(
				name,
				id,
				last_change,
				parent_uuid,
				deleted,
				idle,
				quiet);
	}
	return {};
}

optional<task> task_accessor::get_task_unlimited(const task_id& id)
{
	auto old_task_id = to_id( id);
	sql_statement statement_get_complete_task = db.prepare(R"(
		SELECT
			parent,
			name,
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
	statement_get_complete_task.bind_value( 1, old_task_id);
	Query_result rows = statement_get_complete_task.execute();
	if (rows.size() == 1)
	{
		vector<data_cell> row = rows.at(0);
		int64_t parent = 0;
		if (row[0].has_value())
		{
			parent = row[0].integer();
		}
		auto column{1};
		string   name        = row[column++].text();
		auto     l_uuid      = uuid::from_string( row[column++].text());
		auto     last_change = system_clock::from_time_t(row[column++].integer());
		bool     deleted     = row[column++].boolean();
		auto     idle        = minutes(row[column++].integer());
		bool     quiet       = row[column].boolean();
		if(l_uuid.has_value())
		{
			return task(
					name,
					task_id(*l_uuid),
					last_change,
					to_task_id(parent),
					deleted,
					idle,
					quiet);
		}
	}
	return {};
}

vector<task> task_accessor::changed_since( time_point<system_clock> timestamp )
{
	vector<task> return_value;
	stringstream statement;

	statement << R"(
			SELECT
				parent,
				name,
				uuid,
				changed,
				deleted,
				idle,
				quiet
			FROM
			    tasks
			WHERE
				changed > )"
				<< system_clock::to_time_t( timestamp );

	Query_result rows = db.execute(statement.str());
	for (std::vector<data_cell> row : rows)
	{
		auto column{0};
		Task_id parent = 0;
		if (row[column].has_value())
		{
			parent = (Task_id)row[column].integer();
		}
		column++;
		string  name        = row[column++].text();
		auto    l_uuid      = libtimeit::optional_task_id(row[column++].text());
		auto    last_change = system_clock::from_time_t(row[column++].integer());
		bool    deleted     = row[column++].boolean();
		auto    idle        = minutes(row[column++].integer());
		bool    quiet       = row[column].boolean();
		if (l_uuid.has_value())
		{
			auto parent_uuid= optional_task_id(parent);
			return_value.emplace_back(
					name,
					*l_uuid,
					last_change,
					parent_uuid,
					deleted,
					idle,
					quiet);
		}
	}
	return return_value;
}

Task_id task_accessor::to_id(const task_id& id)
{

	Task_id old_id = 0;
	statement_uuid_to_id.bind_value(1, static_cast<string>(id));
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<data_cell> row : rows)
	{
		old_id = row[0].integer();
	}
	return old_id;
}

Task_id task_accessor::to_id(const uuid& id)
{
	statement_uuid_to_id.bind_value(1, id.to_string());
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<data_cell> row : rows)
	{
		return row[0].integer();
	}
	return 0;
}

optional<task_id> task_accessor::optional_task_id(Task_id id)
{
	if( id>0 )
	{
		statement_id_to_uuid.bind_value(1, id);
		Query_result rows = statement_id_to_uuid.execute();
		for (vector<data_cell> row : rows)
		{
			auto optional_uuid = uuid::from_string( row[0].text() );
			if(optional_uuid.has_value())
			{
				return task_id(optional_uuid.value());
			}
		}
	}
	return {};
}

optional<task_id> task_accessor::to_task_id(Task_id id)
{
	if( id>0 )
	{
		statement_id_to_uuid.bind_value(1, id);
		Query_result rows = statement_id_to_uuid.execute();
		for (vector<data_cell> row : rows)
		{
			return task_id( uuid::from_string( row[0].text()).value());
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
					idle = ?,
					quiet = ?
				WHERE id=?;)"
						);
	auto index{1};
	statement_update_task.bind_value(index++, item.name);

	if (item.parent_id.has_value())
	{
		auto parent_id = to_id(item.parent_id.value());
		statement_update_task.bind_value(index++, parent_id);
	}
	else
	{
		statement_update_task.bind_null_value(index++);
	}
	statement_update_task.bind_value(index++, system_clock::to_time_t(item.last_changed));
	statement_update_task.bind_value(index++, (int64_t)item.deleted);
	statement_update_task.bind_value(index++, item.idle.count());
	statement_update_task.bind_value(index++, (int64_t)item.quiet);
	auto old_id = to_id(item.id);
	statement_update_task.bind_value(index, old_id);

	statement_update_task.execute();
}

void task_accessor::notify(const task &existingTask, const task &item)
{
	bool parent_changed = ( existingTask.parent_id != item.parent_id);
	bool task_deleted   = ((existingTask.deleted != item.deleted) && item.deleted);
	bool name_changed   = (existingTask.name != item.name);

	if (name_changed)
	{
		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_task_name_changed( item );
				});
	}
	if (parent_changed)
	{
		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_parent_changed( item );
				});
	}
	if (task_deleted)
	{
		db.broadcast(
				[item](event_observer* observer)
				{
					observer->on_task_removed( item );
				});
	}
}

bool task_accessor::update(const task &item)
{
	if ( to_id(item.id) == 0)
	{
		stringstream message;
		message << "Unable to find task with UUID=" << static_cast<string>(item.id);
		throw db_exception(message.str());
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

void task_accessor::create(const task &item)
{

	auto index{1};
	statement_new_task.bind_value(index++, item.name);
	if (item.parent_id.has_value())
	{
		auto parent_id = to_id(item.parent_id.value());
		statement_new_task.bind_value(index++, parent_id);
	}
	else
	{
		statement_new_task.bind_null_value(index++);
	}
	statement_new_task.bind_value(index++, system_clock::to_time_t(item.last_changed));
	statement_new_task.bind_value(index++, static_cast<string>(item.id));
	statement_new_task.bind_value(index++, (int64_t)item.deleted);
	statement_new_task.bind_value(index++, item.idle.count());
	statement_new_task.bind_value(index,   (int64_t)item.quiet);

	statement_new_task.execute();

	db.broadcast(
			[item](event_observer* observer)
			{
				observer->on_task_added(item);
			});
}

void task_accessor::internal_create( const task &item )
{
	auto index{1};
	statement_new_task.bind_value(index++, item.name);
	if (item.parent_id.has_value())
	{
		auto parent_id = to_id(item.parent_id.value());
		statement_new_task.bind_value(index++, parent_id);
	}
	else
	{
		statement_new_task.bind_null_value(index++);
	}
	statement_new_task.bind_value(index++, system_clock::to_time_t(item.last_changed));
	statement_new_task.bind_value(index++, static_cast<string>(item.id));
	statement_new_task.bind_value(index++, (int64_t)item.deleted);
	statement_new_task.bind_value(index++, item.idle.count());
	statement_new_task.bind_value(index,   (int64_t)item.quiet);

	statement_new_task.execute();
}

void task_accessor::remove(const task_id& id)
{
	auto item = by_id(id);
	update(item->with_deleted(true));
}



void task_accessor::setup(database& db)
{
	task_accessor::create_table(db);
}

}

