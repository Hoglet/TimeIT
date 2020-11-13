#include <gtest/gtest.h>
#include "TempDB.h"
#include <DB/AutotrackAccessor.h>
#include <vector>
#include <sstream>

using namespace DB;

TEST (AutotrackAccessor, WorkspaceAccessor)
{
	TempDB tempdb;
	std::shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Tjohopp", 0);
	std::vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskId, workspaces);
	std::vector<int> result = autotrackAccessor->getWorkspaces(1);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	TempDB tempdb;
	std::shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	Task task("test");
	int taskID = taskAccessor->newTask(task);
	std::vector<int> workspaces;

	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskID, workspaces);
	std::vector<int64_t> result = autotrackAccessor->getTaskIDs(0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	std::stringstream statement;
	statement << "UPDATE tasks SET deleted = 1 ";
	statement << " ,changed = 0";
	statement << " WHERE id = " << taskID;
	tempdb.execute(statement.str());

	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


