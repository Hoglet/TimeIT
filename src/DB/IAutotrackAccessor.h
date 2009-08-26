/*
 * IAutotrackAccessor.h
 *
 *  Created on: Mar 3, 2009
 *      Author: hoglet
 */

#ifndef IAUTOTRACKACCESSOR_H_
#define IAUTOTRACKACCESSOR_H_
#include <stdint.h>
#include <vector>

class IAutotrackAccessor
{
public:
	virtual std::vector<int64_t> getTaskIDs(int workspace) = 0;
	virtual std::vector<int> getWorkspaces(int64_t taskID) = 0;
	virtual void setWorkspaces(int64_t taskID,std::vector<int> workspaces) = 0;
};

#endif /* IAUTOTRACKACCESSOR_H_ */
