#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_AutotrackAccessor.h"
#include "TempDB.h"
#include "IAutotrackAccessor.h"
#include <vector>
#include "dbexception.h"

namespace Test
{

void test_WorkspaceAccessor()
{
	TempDB tempdb;
	std::shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Tjohopp", 0);
	std::vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(taskId, workspaces);
	std::vector<int> result = autotrackAccessor->getWorkspaces(1);
	ASSERT_EQUAL(result, workspaces);
}


void test_getTaskIDs()
{
	TempDB tempdb;
	std::shared_ptr<IAutotrackAccessor> autotrackAccessor = tempdb.getAutotrackAccessor();
	std::vector<int> workspaces;
	workspaces.push_back(1);
	autotrackAccessor->setWorkspaces(1, workspaces);
	std::vector<int64_t> result = autotrackAccessor->getTaskIDs(0);
	ASSERT_EQUAL(0, result.size());
	result = autotrackAccessor->getTaskIDs(1);
	ASSERT_EQUAL(1, result.size());
}
/*
 virtual std::vector<int64_t> getTaskIDs(int workspace) = 0;
 virtual std::vector<int> getWorkspaces(int64_t taskID) = 0;
 virtual void setWorkspaces(int64_t taskID, std::vector<int> workspaces) = 0;
 */

cute::suite make_suite_test_AutotrackAccessor()
{
	cute::suite s;
	s.push_back(CUTE(test_WorkspaceAccessor));
	s.push_back(CUTE(test_getTaskIDs));
	return s;
}
}

