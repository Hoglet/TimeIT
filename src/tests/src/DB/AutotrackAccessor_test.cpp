#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/autotrack_accessor.h>
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
	Autotrack_accessor autotrackAccessor(tempdb);
	Extended_task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Tjohopp", 0));
	vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor.set_workspaces(taskId, workspaces);
	vector<int> result = autotrackAccessor.workspaces(1);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Autotrack_accessor autotrackAccessor(tempdb);
	Task_accessor taskAccessor(tempdb);
	Task task("test");
	int taskID = taskAccessor.create(task);
	vector<int> workspaces;

	workspaces.push_back(1);
	autotrackAccessor.set_workspaces(taskID, workspaces);
	vector<int64_t> result = autotrackAccessor.task_IDs(0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrackAccessor.task_IDs(1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	stringstream statement;
	statement << "UPDATE tasks SET deleted = 1 ";
	statement << " ,changed = 0";
	statement << " WHERE id = " << taskID;
	tempdb.execute(statement.str());

	result = autotrackAccessor.task_IDs(1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


}