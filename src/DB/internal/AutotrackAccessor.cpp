/*
 * AutotrackAccessor.cpp
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#include "AutotrackAccessor.h"
#include <sstream>
#include "TaskAccessor.h"
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace DBAbstraction;

AutotrackAccessor::AutotrackAccessor(const std::string& dbpath, boost::shared_ptr<TaskAccessor>& taskAccessor): db(dbpath)
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
	try
	{
		stringstream statement;
		statement << "SELECT taskID FROM autotrack where workspace =" << workspace;
		db.exe(statement.str());
		for (unsigned int r = 0; r < db.rows.size(); r++)
		{
			vector<DataCell> row = db.rows[r];
			int64_t id = row[0].getInt();
			retVal.push_back(id);
		}
	} catch (dbexception& e)
	{
		cerr << e.what() << endl;
		throw e;
	}
	return retVal;
}

std::vector<int> AutotrackAccessor::getWorkspaces(int64_t taskID)
{
	std::vector<int> retVal;
	try
	{
		stringstream statement;
		statement << "SELECT workspace FROM autotrack where taskID =" << taskID;
		db.exe(statement.str());
		for (unsigned int r = 0; r < db.rows.size(); r++)
		{
			vector<DataCell> row = db.rows[r];
			int workspace = row[0].getInt();
			retVal.push_back(workspace);
		}
	} catch (dbexception& e)
	{
		cerr << e.what() << endl;
		throw e;
	}
	return retVal;
}

void AutotrackAccessor::setWorkspaces(int64_t taskID, std::vector<int> workspaces)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << taskID;
	try
	{
		db.exe(statement.str());

	} catch (dbexception& e)
	{
		cerr << e.what() << endl;
	}
	std::vector<int>::iterator iter;
	for (iter=workspaces.begin(); iter!=workspaces.end(); iter++)
	{
		int workspace=*iter;
		statement.str("");
		statement << "INSERT INTO autotrack (taskID,workspace) VALUES (" << taskID << ", " << workspace << ")";
		try
		{
			db.exe(statement.str());
		} catch (dbexception& e)
		{
			cerr << e.what() << endl;
		}
	}
}

void AutotrackAccessor::on_taskRemoved(int64_t taskID)
{
	stringstream statement;
	statement << "DELETE FROM autotrack WHERE taskID = " << taskID;
	try
	{
		db.exe(statement.str());

	} catch (dbexception& e)
	{
		cerr << e.what() << endl;
	}
}
