#include "cute.h"
#include "test_extendedTask.h"
#include "ExtendedTask.h"

using namespace DB;

namespace Test
{

void ExtendedTask_testExpanded()
{
	ExtendedTask task(0, 0, "Test");
	ASSERT_EQUALM("Test expanded", 0, task.getExpanded());
	task.setExpanded(1);
	ASSERT_EQUALM("Test expanded", 1, task.getExpanded());
}

void ExtendedTask_testTime()
{
	ExtendedTask task(0, 0, "Test");
	ASSERT_EQUALM("Test time", 0, task.getTime());
	task.setTime(100);
	ASSERT_EQUALM("Test time", 100, task.getTime());
	ASSERT_EQUALM("Test total time", 0, task.getTotalTime());
	task.setTotalTime(100);
	ASSERT_EQUALM("Test total time", 100, task.getTotalTime());
}

void ExtendedTask_testRunning()
{
	ExtendedTask task(0, 0, "Test",0,0,false);
	ExtendedTask task2(0, 0, "Test2",0,0,true);
	ASSERT_EQUALM("Test running", false, task.getRunning());
	ASSERT_EQUALM("Test running (2)", true, task2.getRunning());
}



cute::suite make_suite_ExtendedTask_test()
{
	cute::suite s;
	s.push_back(CUTE(ExtendedTask_testExpanded));
	s.push_back(CUTE(ExtendedTask_testTime));
	s.push_back(CUTE(ExtendedTask_testRunning));
	return s;
}

} /* namespace Test */
