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
	task test_task( "Tjohopp", {} );
	taskAccessor.create( test_task );
	vector<unsigned> workspaces;
	workspaces.push_back(1);
	autotrackAccessor.set_workspaces( test_task.id, workspaces);
	vector<unsigned> result = autotrackAccessor.workspaces(test_task.id);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	auto_track_accessor autotrackAccessor(tempdb);
	task_accessor tasks(tempdb);

	task test_task("test");
	tasks.create( test_task );
	vector<unsigned> workspaces;

	workspaces.push_back(1);
	autotrackAccessor.set_workspaces( test_task.id, workspaces);
	auto result = autotrackAccessor.task_ids(0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrackAccessor.task_ids(1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	tasks.remove(test_task.id);

	result = autotrackAccessor.task_ids(1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


}