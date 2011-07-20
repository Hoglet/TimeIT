/*
 * TestRunner.cpp
 *
 *  Created on: 14 jul 2011
 *      Author: hoglet
 */

#include "TestRunner.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_TaskAccessor.h"

TestRunner::TestRunner()
{
	s += make_suite_test_TaskAccessor();
}

TestRunner::~TestRunner()
{
}

void TestRunner::run()
{
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Main tests");
	exit(0);
}
