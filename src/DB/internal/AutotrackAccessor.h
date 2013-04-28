/*
 * AutotrackAccessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef AUTOTRACKACCESSOR_H_
#define AUTOTRACKACCESSOR_H_
#include <vector>
#include <ITaskAccessor.h>
#include <IAutotrackAccessor.h>
#include "CSQL.h"
#include <ITaskAccessor.h>
#include <memory>

class AutotrackAccessor : public TaskAccessorObserver, public IAutotrackAccessor
{
public:
	AutotrackAccessor(const std::string& dbpath,  std::shared_ptr<ITaskAccessor>& taskAccessor);
	virtual ~AutotrackAccessor();
	std::vector<int64_t> getTaskIDs(int workspace);
	std::vector<int> getWorkspaces(int64_t taskID);
	void setWorkspaces(int64_t taskID,std::vector<int> workspaces);
private:
	virtual void on_taskAdded(int64_t)  {};
	virtual void on_taskUpdated(int64_t) {};
	virtual void on_taskRemoved(int64_t);
	std::shared_ptr<ITaskAccessor> m_taskAccessor;
	DBAbstraction::CSQL db;
};

#endif /* AUTOTRACKACCESSOR_H_ */
