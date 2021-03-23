/*
 * AutotrackAccessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef TIMEIT_AUTOTRACKACCESSOR_H_
#define TIMEIT_AUTOTRACKACCESSOR_H_
#include <vector>
#include "ExtendedTaskAccessor.h"
#include "AutotrackAccessor.h"
#include "CSQL.h"
#include <memory>

namespace libtimeit
{

using namespace std;

class AutotrackAccessor
{
public:
	AutotrackAccessor(Database& db);
	virtual ~AutotrackAccessor();
	vector<int64_t> getTaskIDs(int workspace);
	vector<int> getWorkspaces(int64_t taskID);
	void setWorkspaces(int64_t taskID, vector<int> workspaces);
private:
	ExtendedTaskAccessor task_accessor;
	Database& database;
};

}
#endif /* AUTOTRACKACCESSOR_H_ */
