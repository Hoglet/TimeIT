/*
 * MockTaskAccessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKTASKACCESSOR_H_
#define MOCKTASKACCESSOR_H_

#include "ITaskAccessor.h"

namespace Test
{


class MockTaskAccessor: public ITaskAccessor
{
public:
	MockTaskAccessor();
	virtual ~MockTaskAccessor();
	virtual void attach(TaskAccessorObserver*);
	virtual void detach(TaskAccessorObserver*);

	virtual std::shared_ptr<std::vector<Task>> getTask(int64_t taskID, time_t start = 0, time_t stop = 0, bool calculateTotalTime=true);
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
	virtual std::shared_ptr<std::vector<Task>> getRunningTasks(int64_t parentID = 0);
	virtual int64_t newTask(std::string name, int64_t parentID);
	virtual void setTaskExpanded(int64_t taskID, bool expanded);
	virtual void setTaskName(int64_t taskID, std::string name);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
};

} /* namespace Test */
#endif /* MOCKTASKACCESSOR_H_ */
