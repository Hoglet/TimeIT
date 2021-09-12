#include "legacy_db_helper.h"

namespace legacy_db_helper
{

Task_id to_id(const task_id& id, database& db)
{

	Task_id old_id = 0;
	sql_statement statement_uuid_to_id(db.prepare("SELECT id FROM tasks WHERE uuid=?;"));
	statement_uuid_to_id.bind_value(1, static_cast<string>(id));
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<data_cell> row : rows)
	{
		old_id = row[0].integer();
	}
	return old_id;
}

Task_id to_id(const uuid& id, database& db)
{
	sql_statement statement_uuid_to_id(db.prepare("SELECT id FROM tasks WHERE uuid=?;"));
	statement_uuid_to_id.bind_value(1, id.to_string());
	Query_result rows = statement_uuid_to_id.execute();
	for (vector<data_cell> row : rows)
	{
		return row[0].integer();
	}
	return 0;
}


Task_id new_task_id_to_old(task_id owner_id, database& db)
{
	auto statement = db.prepare(R"(
	SELECT
		id
	FROM
		tasks
	WHERE
		uuid = ?
	)");
	statement.bind_value(1,static_cast<string>(owner_id));
	auto rows = statement.execute();
	for( auto row: rows)
	{
		return row[0].integer();
	}
	return 0;
}


optional<task_id> old_task_id_to_new(int64_t owner_id, database& db)
{
	auto statement = db.prepare(R"(
		SELECT
			uuid
		FROM
			tasks
		WHERE
			id = ?
		)");
	statement.bind_value(1,owner_id);
	auto rows = statement.execute();
	for( auto row: rows)
	{
		auto id = uuid::from_string( row[0].text());
		if( id.has_value() )
		{
			return task_id( id.value());
		}
	}
	return {};
}

}

