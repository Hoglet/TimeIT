#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_AutotrackAccessor.h"
#include "TempDB.h"
#include "AutotrackAccessor.h"
#include <vector>
#include "dbexception.h"
#include <sstream>

using namespace DB;

namespace Test
{

void AutotrackAccessor_WorkspaceAccessor()
{
	TempDB tempdb;
	std::shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	std::shared_ptr<IExtendedTaskAccessor> taskAccessor = tempdb.getExtendedTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Tjohopp", 0);
	std::vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskId, workspaces);
	std::vector<int> result = autotrackAccessor->getWorkspaces(1);
	ASSERT_EQUAL(result, workspaces);
}


void AutotrackAccessor_getTaskIDs()
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
	ASSERT_EQUALM("Number of ids on workspace 0 ", 0, result.size());
	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQUALM("Number of ids on workspace 1 ", 1, result.size());

	std::stringstream statement;
	statement << "UPDATE tasks SET deleted = 1 ";
	statement << " ,changed = 0";
	statement << " WHERE id = " << taskID;
	tempdb.execute(statement.str());

	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQUALM("Number of ids on workspace 1 when task is deleted ", 0, result.size());
}

cute::suite make_suite_test_AutotrackAccessor()
{
	cute::suite s;
	s.push_back(CUTE(AutotrackAccessor_WorkspaceAccessor));
	s.push_back(CUTE(AutotrackAccessor_getTaskIDs));
	return s;
}
}

