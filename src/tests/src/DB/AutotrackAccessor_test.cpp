#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/autotrack_accessor.h>
#include <vector>

namespace test
{
using namespace libtimeit;
using namespace std;

TEST (AutotrackAccessor, WorkspaceAccessor)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	auto_track_accessor autotrack_accessor( tempdb);
	extended_task_accessor tasks( tempdb);
	task test_task( "Tjohopp", {} );
	tasks.create( test_task );
	vector<unsigned> workspaces;
	workspaces.push_back(1);
	autotrack_accessor.set_workspaces( test_task.id, workspaces);
	vector<unsigned> result = autotrack_accessor.workspaces( test_task.id);
	ASSERT_EQ(result, workspaces);
}


TEST (AutotrackAccessor, getTaskIDs)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	auto_track_accessor autotrack_accessor( tempdb);
	task_accessor tasks(tempdb);

	task test_task("test");
	tasks.create( test_task );
	vector<unsigned> workspaces;

	workspaces.push_back(1);
	autotrack_accessor.set_workspaces( test_task.id, workspaces);
	auto result = autotrack_accessor.task_ids( 0);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 0 ";
	result = autotrack_accessor.task_ids( 1);
	ASSERT_EQ(1, result.size()) << "Number of ids on workspace 1 ";

	tasks.remove(test_task.id);

	result = autotrack_accessor.task_ids( 1);
	ASSERT_EQ(0, result.size()) << "Number of ids on workspace 1 when task is deleted ";
}


}
