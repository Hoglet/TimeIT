/*
 * AutotrackAccessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef AUTOTRACKACCESSOR_H_
#define AUTOTRACKACCESSOR_H_
#include <vector>
#include "ExtendedTaskAccessor.h"
#include "AutotrackAccessor.h"
#include "CSQL.h"
#include <memory>

namespace DB
{

class IAutotrackAccessor
{
public:
	virtual ~IAutotrackAccessor();
	virtual std::vector<int64_t> getTaskIDs(int workspace) = 0;
	virtual std::vector<int> getWorkspaces(int64_t taskID) = 0;
	virtual void setWorkspaces(int64_t taskID, std::vector<int> workspaces) = 0;
};

class AutotrackAccessor : public IAutotrackAccessor
{
public:
	AutotrackAccessor(std::shared_ptr<DBAbstraction::CSQL>& db,  std::shared_ptr<IExtendedTaskAccessor>& taskAccessor);
	virtual ~AutotrackAccessor();
	std::vector<int64_t> getTaskIDs(int workspace);
	std::vector<int> getWorkspaces(int64_t taskID);
	void setWorkspaces(int64_t taskID,std::vector<int> workspaces);
private:
	std::shared_ptr<IExtendedTaskAccessor> m_taskAccessor;
	std::shared_ptr<DBAbstraction::CSQL> db;
};

}
#endif /* AUTOTRACKACCESSOR_H_ */
