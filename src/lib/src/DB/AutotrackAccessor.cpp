/*
 * AutotrackAccessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include <sstream>
#include "libtimeit/db/AutotrackAccessor.h"
#include "libtimeit/db/ExtendedTaskAccessor.h"

namespace libtimeit
{
using namespace std;

AutotrackAccessor::AutotrackAccessor(Database& op_database) : database(op_database), task_accessor(op_database)
{
}

AutotrackAccessor::~AutotrackAccessor()
{
}

std::vector<int64_t> AutotrackAccessor::getTaskIDs(int workspace)
{
	std::vector<int64_t> retVal;
	stringstream statement;
	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	QueryResult rows = database.exe(statement.str());
	for (vector<DataCell> row : rows)
	{
		int64_t id = row[0].getInt();
		auto task = task_accessor.getTask(id);
		if(task.has_value() && task->deleted() == false)
		{
			retVal.push_back(id);
		}
	}
	return retVal;
}

std::vector<int> AutotrackAccessor::getWorkspaces(int64_t taskID)
{
	std::vector<int> retVal;
	stringstream statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
	QueryResult rows = database.exe(statement.str());
	for (vector<DataCell> row : rows)
	{
		int workspace = row[0].getInt();
		retVal.push_back(workspace);
	}
	return retVal;
}

void AutotrackAccessor::setWorkspaces(int64_t taskID, std::vector<int> workspaces)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << taskID;
	database.exe(statement.str());

	for (int workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << taskID << ", " << workspace << ")";
		database.exe(statement.str());
	}
}

}
