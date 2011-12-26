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
#include <boost/shared_ptr.hpp>
#include "CSQL.h"
#include "TaskAccessor.h"

class AutotrackAccessor : public TaskAccessorObserver, public IAutotrackAccessor
{
public:
	AutotrackAccessor(const std::string& dbpath,  boost::shared_ptr<TaskAccessor>& taskAccessor);
	virtual ~AutotrackAccessor();
	std::vector<int64_t> getTaskIDs(int workspace);
	std::vector<int> getWorkspaces(int64_t taskID);
	void setWorkspaces(int64_t taskID,std::vector<int> workspaces);
private:
	virtual void on_taskAdded(int64_t)  {};
	virtual void on_taskUpdated(int64_t) {};
	virtual void on_taskRemoved(int64_t);
	boost::shared_ptr<ITaskAccessor> m_taskAccessor;
	DBAbstraction::CSQL db;
};

#endif /* AUTOTRACKACCESSOR_H_ */
