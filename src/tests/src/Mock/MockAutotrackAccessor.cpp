/*
 * MockAutotrackAccessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockAutotrackAccessor.h"

namespace test
{

MockAutotrackAccessor::MockAutotrackAccessor()
{
}

MockAutotrackAccessor::~MockAutotrackAccessor()
{
}

std::vector<int64_t> MockAutotrackAccessor::getTaskIDs(int workspace)
{
	std::vector<int64_t> list;
	return list;
}

std::vector<int> MockAutotrackAccessor::getWorkspaces(int64_t taskID)
{
	std::vector<int> list;
	return list;
}

void MockAutotrackAccessor::setWorkspaces(int64_t taskID, std::vector<int> workspaces)
{

}


} /* namespace Test */
