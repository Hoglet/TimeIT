/*
 * TestRunner.h
 *
 *  Created on: 14 jul 2011
 *      Author: hoglet
 */

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "cute.h"


class TestRunner
{
public:
	TestRunner();
	virtual ~TestRunner();
	void run();
private:
	cute::suite s;
};

#endif /* TESTRUNNER_H_ */
