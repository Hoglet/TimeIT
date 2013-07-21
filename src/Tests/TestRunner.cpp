/*
 * TestRunner.cpp
 *
 *  Created on: 14 jul 2011
 *      Author: hoglet
 */

#include "TestRunner.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "Logic/TimerTest.h"
#include "Logic/AutoTrackerTest.h"
#include "DB/test_TimeManagement.h"
#include "DB/Notifier_test.h"
#include "DB/test_TimeAccessor.h"
#include "DB/test_TaskAccessor.h"
#include "DB/test_SettingsAccessor.h"
#include "DB/test_AutotrackAccessor.h"
#include "Misc/test_utils.h"

namespace Test
{
TestRunner::TestRunner()
{
	s += make_suite_test_TimeManagement();
	s += make_suite_Notifier_test();
	s += make_suite_TimerTest();
	s += make_suite_AutoTrackerTest();
	s += make_suite_TimeAccessor_test();
	s += make_suite_test_TaskAccessor();
	s += make_suite_test_SettingsAccessor();
	s += make_suite_test_AutotrackAccessor();
	s += make_suite_UtilsTest();
}

void TestRunner::run()
{
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Main tests");
	exit(0);
}
}
