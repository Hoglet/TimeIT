/*
 * AutotrackAccessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include "AutotrackAccessor.h"
#include <sstream>
#include "ExtendedTaskAccessor.h"

using namespace std;
using namespace DBAbstraction;
namespace DB
{

IAutotrackAccessor::~IAutotrackAccessor()
{
}

AutotrackAccessor::AutotrackAccessor(shared_ptr<CSQL>& op_db, std::shared_ptr<IExtendedTaskAccessor>& taskAccessor)
{
	db = op_db;
	m_taskAccessor = taskAccessor;
}

AutotrackAccessor::~AutotrackAccessor()
{
}

std::vector<int64_t> AutotrackAccessor::getTaskIDs(int workspace)
{
	std::vector<int64_t> retVal;
	stringstream statement;
	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (vector<DataCell> row : *rows)
	{
		int64_t id = row[0].getInt();
		if(m_taskAccessor->getTask(id)->deleted() == false)
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
	std::shared_ptr<QueryResult> rows = db->exe(statement.str());
	for (vector<DataCell> row : *rows)
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
	db->exe(statement.str());

	for (int workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << taskID << ", " << workspace << ")";
		db->exe(statement.str());
	}
}

}
