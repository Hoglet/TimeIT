#ifndef ITASKACCESSOR_H_
#define ITASKACCESSOR_H_

#include <vector>
#include <stdint.h>
#include <memory>
#include "Task.h"

namespace DB
{
class TaskAccessorObserver
{
public:
	virtual ~TaskAccessorObserver();
	virtual void on_taskAdded(int64_t)  = 0;
	virtual void on_taskUpdated(int64_t) = 0;
	virtual void on_taskRemoved(int64_t) = 0;
	virtual void on_taskParentChanged(int64_t);
	virtual void on_completeUpdate() = 0;
};

class ITaskAccessor
{
public:
	virtual ~ITaskAccessor();
	virtual void attach(TaskAccessorObserver*) = 0;
	virtual void detach(TaskAccessorObserver*) = 0;

	virtual std::shared_ptr<Task> getTask(int64_t taskID) = 0;
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0) = 0;
	virtual std::shared_ptr<std::vector<Task>> getTasksChangedSince(time_t timestamp = 0) = 0;
	virtual int64_t newTask(const Task& task) = 0;
	virtual bool updateTask(const Task& task) = 0;
	virtual int64_t newTask(std::string name, int64_t parentID) = 0;
	virtual void setParentID(int64_t taskID, int parentID) = 0;
	virtual void removeTask(int64_t taskID) = 0;
	virtual int64_t uuidToId(std::string uuid) = 0;
	virtual void enableNotifications(bool) = 0;
	virtual void createStatements() = 0;

};

}

#endif /* ITASKACCESSOR_H_ */
