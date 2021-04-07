/*
 * Autotrack_accessor.cpp
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

Autotrack_accessor::Autotrack_accessor(Database& op_database) : database(op_database), task_accessor(op_database)
{
}


void  Autotrack_accessor::create_table()
{

}

void  Autotrack_accessor::upgrade()
{

}

Task_ID_list Autotrack_accessor::task_IDs(int workspace)
{
	Task_ID_list return_value;
	stringstream  statement;

	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	Query_result rows = database.execute(statement.str());
	for (vector<Data_cell> row : rows)
	{
		int64_t id = row[0].integer();
		auto task = task_accessor.by_ID(id);
		if(task.has_value() && task->deleted() == false)
		{
			return_value.push_back(id);
		}
	}
	return return_value;
}

vector<int> Autotrack_accessor::workspaces(int64_t taskID)
{
	vector<int> retVal;
	stringstream     statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
	Query_result rows = database.execute(statement.str());
	for (auto row : rows)
	{
		int workspace = row[0].integer();
		retVal.push_back(workspace);
	}
	return retVal;
}

void Autotrack_accessor::set_workspaces(int64_t task_ID, vector<int> workspaces)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << task_ID;
	database.execute(statement.str());

	for (int workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << task_ID << ", " << workspace << ")";
		database.execute(statement.str());
	}
}

}
