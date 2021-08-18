#include <iostream>
#include <sstream>
#include <libtimeit/db/extended_task_accessor.h>
#include <libtimeit/misc/contains.h>

using namespace std;
namespace libtimeit
{

const auto EXPANDED_QUERY = R"(
		SELECT
			expanded
		FROM
			tasks
		WHERE uuid=?)";



extended_task_accessor::extended_task_accessor(database& db )
		:
		task_accessor(db),
		times(db),
		expanded_statement(db.prepare(EXPANDED_QUERY))
{
}




vector<extended_task> extended_task_accessor::by_parent_id(optional<task_id> id)
{
	auto tasks = task_accessor::by_parent_id(id);

	return from_tasks(tasks);
}

optional<extended_task> extended_task_accessor::by_id(const task_id& id)
{
	auto possible_item = task_accessor::by_id(id);
	if(possible_item.has_value())
	{
		auto running_tasks = times.currently_running();
		return from_task( possible_item.value(), running_tasks );
	}
	return {};
}


void extended_task_accessor::drop_views(database& db)
{
	db.execute("DROP VIEW IF EXISTS v_running");
	db.execute("DROP VIEW IF EXISTS v_tasks");
}

void extended_task_accessor::create_views(database& db)
{
	db.execute(
			R"Query(
				CREATE VIEW
					v_running
				AS SELECT
					taskID,
					running
				FROM
				    times
				WHERE running ="1";
			)Query");
	db.execute(
			R"Query(
				CREATE VIEW
					v_tasks
				AS SELECT
					tasks.*,
					IFNULL(v_running.running,0) as running
				FROM
					tasks
				LEFT JOIN
					v_running
				ON
					tasks.id=v_running.taskId
				)Query");

}

void extended_task_accessor::upgrade(database& /*database*/)
{
	//noop
}

void extended_task_accessor::setup(database& db)
{
	extended_task_accessor::drop_views(db);
	extended_task_accessor::upgrade(db);
	extended_task_accessor::create_views(db);
}


extended_task extended_task_accessor::from_task( const task &item, vector<task_id> running_tasks )
{
	auto time = times.duration_time( item.id ).count();
	auto total_time = times.total_cumulative_time( item.id ).count();

	bool running = contains( running_tasks, item.id);
	bool expanded = is_expanded( item );

	return {
			item,
			time,
			total_time,
			expanded,
			running
	};

}

vector<extended_task> extended_task_accessor::from_tasks(const vector<task>& tasks)
{
	vector<extended_task> return_value = {};
	auto running_tasks = times.currently_running();
	for (auto item: tasks)
	{
		return_value.emplace_back( from_task( item, running_tasks ) );
	}
	return return_value;
}

bool extended_task_accessor::is_expanded(const task& task)
{
	expanded_statement.bind_value(1, static_cast<string>(task.id));
	auto rows = expanded_statement.execute();
	for ( auto row: rows )
	{
		return static_cast<bool>(row[0].integer());
	}
	return false;
}


void extended_task_accessor::set_task_expanded(const task_id& id, bool expanded)
{
	auto statement = db.prepare(
			R"Query(
				UPDATE
					tasks
				SET
					expanded = ?
				WHERE
					uuid= ? ;
			)Query"
			);
	statement.bind_value(1, static_cast<int64_t>(expanded));
	statement.bind_value(2, static_cast<string>( id ) );
	statement.execute();
}


}
