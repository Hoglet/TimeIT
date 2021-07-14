/*
 * Auto_track_accessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include <sstream>
#include "libtimeit/db/autotrack_accessor.h"
#include "libtimeit/db/extended_task_accessor.h"

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

	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	Query_result rows = db.execute(statement.str());
	for (vector<data_cell> row : rows)
	{
		int64_t id = row[0].integer();
		auto item = tasks.by_id(id);
		if (item.has_value() && !item->deleted)
		{
			return_value.push_back(id);
		}
	}
	return return_value;
}

vector<unsigned> auto_track_accessor::workspaces(int64_t taskID)
{
	vector<unsigned> return_value;
	stringstream     statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
	Query_result rows = db.execute(statement.str());
	for (auto row : rows)
	{
		auto workspace = (unsigned)row[0].integer();
		return_value.push_back(workspace);
	}
	return return_value;
}

void auto_track_accessor::set_workspaces(int64_t task_ID, vector<unsigned> workspaces)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << task_ID;
	db.execute(statement.str());

	for (auto workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << task_ID << ", " << workspace << ")";
		db.execute(statement.str());
	}
}

void auto_track_accessor::setup(database& db)
{
	auto_track_accessor::create_table(db);
	auto_track_accessor::upgrade(db);

}

}
