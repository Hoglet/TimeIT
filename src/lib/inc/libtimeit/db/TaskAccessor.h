#ifndef TASKACCESSOR_H_
#define TASKACCESSOR_H_

#include "Database.h"
#include "CSQL.h"
#include "libtimeit/EventObserver.h"
#include "Task.h"

#include "Notifier.h"
#include <string>


namespace libtimeit
{

class TaskAccessor
{
	friend class Database;
	friend class SyncManager;

public:
	TaskAccessor(Database& database);
	virtual ~TaskAccessor();
	virtual void attach(EventObserver*);
	virtual void detach(EventObserver*);
	virtual optional<Task> getTask(int64_t taskID);
	virtual vector<Task> getTasks(int64_t parentID = 0);
	virtual vector<Task> getTasksChangedSince(time_t timestamp = 0);
	virtual int64_t newTask(const Task &task);
	virtual bool updateTask(const Task &task);
	virtual int64_t newTask(string name, int64_t parentID);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
	virtual int64_t uuidToId(UUID uuid);
	virtual void enableNotifications(bool);
	virtual void setTaskExpanded(int64_t taskID, bool expanded);

protected:
	void createTable();
	void upgradeToDB5();
	Database& database;

private:
	void notify(const Task &oldTask, const Task &newTask);
	void _update(const Task &task);

	dbexception          dbe;
	optional<class UUID> idToUuid(int64_t id);
	optional<Task>       getTaskUnlimited(int64_t taskID);
};

}
#endif /* TASKACCESSOR_H_ */
