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

Auto_track_accessor::Auto_track_accessor(Database& op_database) : database(op_database), task_accessor(op_database)
{
}


void  Auto_track_accessor::create_table(Database& database)
{
	database.execute(R"Query(
		CREATE TABLE IF NOT EXISTS
			autotrack
			(
				taskID    INTEGER,
				workspace INTEGER
			)
		)Query");
}

void  Auto_track_accessor::upgrade(Database& /*database*/)
{

}

Task_id_list Auto_track_accessor::task_ids(unsigned workspace)
{
	Task_id_list return_value;
	stringstream  statement;

	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
	{
		int64_t id = row[0].integer();
		auto task = task_accessor.by_id(id);
		if (task.has_value() && !task->deleted)
		{
			return_value.push_back(id);
		}
	}
	return return_value;
}

vector<unsigned> Auto_track_accessor::workspaces(int64_t taskID)
{
	vector<unsigned> return_value;
	stringstream     statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
	Query_result rows = database.execute(statement.str());
	for (auto row : rows)
	{
		auto workspace = (unsigned)row[0].integer();
		return_value.push_back(workspace);
	}
	return return_value;
}

void Auto_track_accessor::set_workspaces(int64_t task_ID, vector<unsigned> workspaces)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << task_ID;
	database.execute(statement.str());

	for (auto workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << task_ID << ", " << workspace << ")";
		database.execute(statement.str());
	}
}

void Auto_track_accessor::setup(Database& database)
{
	Auto_track_accessor::create_table(database);
	Auto_track_accessor::upgrade(database);

}

}
