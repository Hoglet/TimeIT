/*
 * AutotrackAccessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include "AutotrackAccessor.h"
#include <sstream>
#include "TaskAccessor.h"

using namespace std;
using namespace DBAbstraction;

AutotrackAccessor::AutotrackAccessor(const std::string& dbpath, std::shared_ptr<ITaskAccessor>& taskAccessor) :
		db(dbpath)
{
	m_taskAccessor = taskAccessor;
	m_taskAccessor->attach(this);
}

AutotrackAccessor::~AutotrackAccessor()
{
	m_taskAccessor->detach(this);
}
std::vector<int64_t> AutotrackAccessor::getTaskIDs(int workspace)
{
	std::vector<int64_t> retVal;
	stringstream statement;
	statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
	db.exe(statement.str());
	for (vector<DataCell> row : db.rows)
	{
		int64_t id = row[0].getInt();
		retVal.push_back(id);
	}
	return retVal;
}

std::vector<int> AutotrackAccessor::getWorkspaces(int64_t taskID)
{
	std::vector<int> retVal;
	stringstream statement;
	statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
	db.exe(statement.str());
	for (vector<DataCell> row : db.rows)
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
	db.exe(statement.str());

	for (int workspace : workspaces)
	{
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << taskID << ", " << workspace << ")";
		db.exe(statement.str());
	}
}

void AutotrackAccessor::on_taskRemoved(int64_t taskID)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << taskID;
	db.exe(statement.str());
}
