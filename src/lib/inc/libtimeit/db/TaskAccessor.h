#ifndef TASKACCESSOR_H_
#define TASKACCESSOR_H_

#include "CSQL.h"
#include "libtimeit/EventObserver.h"
#include "Task.h"

#include "Notifier.h"
#include <string>

namespace libtimeit
{

class ITaskAccessor
{
public:
	virtual ~ITaskAccessor();
	virtual void attach(EventObserver*) = 0;
	virtual void detach(EventObserver*) = 0;

	virtual std::shared_ptr<Task> getTask(int64_t taskID) = 0;
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0) = 0;
	virtual std::shared_ptr<std::vector<Task>> getTasksChangedSince(time_t timestamp = 0) = 0;
	virtual int64_t newTask(const Task &task) = 0;
	virtual bool updateTask(const Task &task) = 0;
	virtual int64_t newTask(std::string name, int64_t parentID) = 0;
	virtual void setParentID(int64_t taskID, int parentID) = 0;
	virtual void removeTask(int64_t taskID) = 0;
	virtual int64_t uuidToId(UUID uuid) = 0;
	virtual void enableNotifications(bool) = 0;
	virtual void setTaskExpanded(int64_t taskID, bool expanded) = 0;

};

class Database;

class TaskAccessor: public virtual ITaskAccessor
{
	friend class Database;
	friend class SyncManager;

public:
	TaskAccessor(std::shared_ptr<CSQL> &op_db, Notifier& notifier);
	virtual ~TaskAccessor();
	virtual void attach(EventObserver*);
	virtual void detach(EventObserver*);
	virtual std::shared_ptr<Task> getTask(int64_t taskID);
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0);
	virtual std::shared_ptr<std::vector<Task>> getTasksChangedSince(time_t timestamp = 0);
	virtual int64_t newTask(const Task &task);
	virtual bool updateTask(const Task &task);
	virtual int64_t newTask(std::string name, int64_t parentID);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
	virtual int64_t uuidToId(UUID uuid);
	virtual void enableNotifications(bool);
	virtual void setTaskExpanded(int64_t taskID, bool expanded);

protected:
	void createTable();
	void upgradeToDB5();
	Notifier& notifier;
	std::shared_ptr<CSQL> db;

private:
	void notify(const Task &oldTask, const Task &newTask);
	void _update(const Task &task);

	dbexception          dbe;
	optional<class UUID> idToUuid(int64_t id);
	Task                 getTaskUnlimited(int64_t taskID);
};

}
#endif /* TASKACCESSOR_H_ */
