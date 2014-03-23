#ifndef TASKACCESSOR_H_
#define TASKACCESSOR_H_

#include "CSQL.h"
#include "ITaskAccessor.h"

#include <Notifier.h>
#include <string>

namespace DB
{

class Database;

class TaskAccessor : public virtual ITaskAccessor
{
	friend class Database;
	friend class SyncManager;
public:
	TaskAccessor(std::shared_ptr<DBAbstraction::CSQL>& op_db, std::shared_ptr<Notifier> notifier);
	virtual ~TaskAccessor();
	virtual void attach(TaskAccessorObserver*);
	virtual void detach(TaskAccessorObserver*);
	virtual std::shared_ptr<Task> getTask(int64_t taskID);
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0) ;
	virtual std::shared_ptr<std::vector<Task>> getTasksChangedSince(time_t timestamp = 0) ;
	virtual int64_t newTask(const Task& task);
	virtual bool updateTask(const Task& task);
	virtual int64_t newTask(std::string name, int64_t parentID);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
	virtual int64_t uuidToId(std::string uuid);
	virtual void enableNotifications(bool);
	virtual void createStatements();
protected:
	void createTable();
	void upgradeToDB5();
	std::shared_ptr<Notifier> notifier;
	std::shared_ptr<DBAbstraction::CSQL> db;
	dbexception dbe;
	std::string idToUuid(int64_t id);
	std::shared_ptr<Task> getTaskUnlimited(int64_t taskID);


	std::shared_ptr<DBAbstraction::Statement> statement_uuidToId;
	std::shared_ptr<DBAbstraction::Statement> statement_getTask;
	std::shared_ptr<DBAbstraction::Statement> statement_getCompleteTask;
	std::shared_ptr<DBAbstraction::Statement> statement_idToUuid;
	std::shared_ptr<DBAbstraction::Statement> statement_newTask;
	std::shared_ptr<DBAbstraction::Statement> statement_updateTask;
};

}
#endif /* TASKACCESSOR_H_ */
