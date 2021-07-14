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
	notification_manager notifier;
	TempDB tempdb(notifier);
	auto_track_accessor autotrackAccessor(tempdb);
	extended_task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(task("Tjohopp", 0));
	vector<unsigned> workspaces;
	workspaces.push_back(1);
	autotrackAccessor.set_workspaces(taskId, workspaces);
	vector<unsigned> result = autotrackAccessor.workspaces(1);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	auto_track_accessor autotrackAccessor(tempdb);
	task_accessor tasks(tempdb);
	int taskID = tasks.create(task("test") );
	vector<unsigned> workspaces;

	workspaces.push_back(1);
	autotrackAccessor.set_workspaces(taskID, workspaces);
	vector<int64_t> result = autotrackAccessor.task_ids(0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrackAccessor.task_ids(1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	stringstream statement;
	statement << "UPDATE tasks SET deleted = 1 ";
	statement << " ,changed = 0";
	statement << " WHERE id = " << taskID;
	tempdb.execute(statement.str());

	result = autotrackAccessor.task_ids(1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


}