/*
 * Auto_track_accessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include <sstream>
#include "libtimeit/db/autotrack_accessor.h"
#include "legacy_db_helper.h"

namespace libtimeit
{
using namespace std;

auto_track_accessor::auto_track_accessor(database& op_database)
	:
		tasks(op_database),
		db(op_database)
{
}


void  auto_track_accessor::create_table(database& db)
{
	db.execute(R"Query(
		CREATE TABLE IF NOT EXISTS
			autotrack
			(
				taskID    INTEGER,
				workspace INTEGER
			)
		)Query");
}

void  auto_track_accessor::upgrade(database& /*database*/)
{

}

task_id_list auto_track_accessor::task_ids(unsigned workspace)
{
	task_id_list return_value;
	stringstream  statement;

	statement << "SELECT taskID FROM autotrack WHERE workspace =" << workspace;
	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
	{
		int64_t old_id = row[0].integer();
		auto id = legacy_db_helper::old_task_id_to_new(old_id, db);
		if( id.has_value() )
		{
			auto item = tasks.by_id(id.value());
			if (item.has_value() && !item->deleted)
			{
				return_value.push_back(item->id);
			}
		}
	}
	return return_value;
}

vector<unsigned> auto_track_accessor::workspaces(const task_id& id)
{
	auto task_id = legacy_db_helper::new_task_id_to_old( id, db);
	vector<unsigned> return_value;
	stringstream     statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << task_id;
	Query_result rows = db.execute(statement.str());
	for (auto row : rows)
	{
		auto workspace = (unsigned)row[0].integer();
		return_value.push_back(workspace);
	}
	return return_value;
}

void auto_track_accessor::set_workspaces(const task_id& id, const vector<unsigned>& workspaces)
{
	auto task_id = legacy_db_helper::new_task_id_to_old( id, db);
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << task_id;
	db.execute(statement.str());

	for (auto workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << task_id << ", " << workspace << ")";
		db.execute(statement.str());
	}
}

void auto_track_accessor::setup(database& db)
{
	auto_track_accessor::create_table(db);
	auto_track_accessor::upgrade(db);

}

}
