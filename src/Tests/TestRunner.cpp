
#include "TestRunner.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "Logic/AutoTrackerTest.h"
#include "Logic/UUID_test.h"
#include "Logic/ControllerTest.h"
#include "Logic/TimeKeeperTest.h"
#include "Misc/test_exceptions.h"
#include "Misc/test_OSAbstraction.h"
#include "Misc/test_ApplicationLock.h"
#include "Misc/test_utils.h"
#include "Misc/TimerTest.h"
#include "DB/test_TimeManagement.h"
#include "DB/Notifier_test.h"
#include "DB/test_TimeAccessor.h"
#include "DB/test_ExtendedTaskAccessor.h"
#include "DB/test_TaskAccessor.h"
#include "DB/test_SettingsAccessor.h"
#include "DB/test_AutotrackAccessor.h"
#include "Misc/test_utils.h"
#include "DB/test_task.h"
#include "DB/test_extendedTask.h"
#include "DB/test_db.h"
#include "DB/test_timeEntry.h"
#include "Sync/test_json.h"
#include "Sync/SyncTest.h"

namespace Test
{
TestRunner::TestRunner()
{
	s += make_suite_TimeKeeperTest();
	s += make_suite_ControllerTest();
	s += make_suite_UUIDTest();
	s += make_suite_AutoTrackerTest();
	s += make_suite_test_TimeManagement();
	s += make_suite_Notifier_test();
	s += make_suite_TimerTest();
	s += make_suite_AutoTrackerTest();
	s += make_suite_TimeAccessor_test();
	s += make_suite_test_TaskAccessor();
	s += make_suite_test_ExtendedTaskAccessor();
	s += make_suite_test_SettingsAccessor();
	s += make_suite_test_AutotrackAccessor();
	s += make_suite_UUIDTest();
	s += make_suite_test_TaskAccessor();
	s += make_suite_Task_test();
	s += make_suite_exceptionTest();
	s += make_suite_OSAbstractionTest();
	s += make_suite_ExtendedTask_test();
	s += make_suite_DB_test();
	s += make_suite_ApplicationLockTest();
	s += make_suite_UtilsTest();
	s += make_suite_JsonTest();
	s += make_suite_SyncTest();
	s += make_suite_test_TimeEntry();
}

void TestRunner::run()
{
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Main tests");
	exit(0);
}
}
