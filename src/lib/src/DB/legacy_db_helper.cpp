#include "legacy_db_helper.h"

namespace legacy_db_helper
{
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
		auto uuid = UUID::from_string(row[0].text());
		if( uuid.has_value() )
		{
			return task_id(uuid.value());
		}
	}
	return {};
}

}

