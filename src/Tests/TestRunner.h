/*
 * TestRunner.h
 *
 *  Created on: 14 jul 2011
 *      Author: hoglet
 */

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "cute.h"

namespace Test
{

class TestRunner
{
public:
	TestRunner();
	void run(int argc, char *argv[]);
private:
	cute::suite s;
};

}

#endif /* TESTRUNNER_H_ */
