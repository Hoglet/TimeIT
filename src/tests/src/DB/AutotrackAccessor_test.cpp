#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/AutotrackAccessor.h>
#include <vector>
#include <sstream>

namespace test
{
using namespace libtimeit;
using namespace std;

TEST (AutotrackAccessor, WorkspaceAccessor)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Tjohopp", 0);
	vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskId, workspaces);
	vector<int> result = autotrackAccessor->getWorkspaces(1);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	Task task("test");
	int taskID = taskAccessor->newTask(task);
	vector<int> workspaces;

	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskID, workspaces);
	vector<int64_t> result = autotrackAccessor->getTaskIDs(0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	stringstream statement;
	statement << "UPDATE tasks SET deleted = 1 ";
	statement << " ,changed = 0";
	statement << " WHERE id = " << taskID;
	tempdb.execute(statement.str());

	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


}