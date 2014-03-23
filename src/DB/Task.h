#ifndef TASK_H_
#define TASK_H_
#include <string>
#include "UUID.h"

namespace Test
{
class TestTaskC;
class Network_test;
}
;

namespace DB
{
class TaskAccessor;

class Task
{
	friend class TaskAccessor;
	friend class SyncManager;
public:
	Task(const std::string& name, int64_t parentID = 0);
	Task(const std::string& name, int64_t parentID, const std::string& uuid, bool completed, int64_t ID, time_t lastChange, const std::string& parentUUID, bool deleted);
	virtual ~Task();
	std::string getName() const;
	void setName(std::string newName);
	int64_t getID() const;
	int64_t getParentID() const;
	void setParent(int64_t);
	bool getCompleted() const;
	void setCompleted(bool);
	std::string getUUID() const;
	std::string getParentUUID() const;
	time_t getLastChanged() const;
	bool getDeleted() const;
	void setDeleted(bool state);
	friend bool operator== (const Task &op1, const Task &op2);
	friend bool operator!= (const Task &op1, const Task &op2);

protected:
	std::string name;
	int64_t ID;
	int64_t parentID;
	std::string uuid;
	std::string parentUuid;
	bool completed;
	time_t lastChanged;
	bool deleted;
	Task();
	static UUID uuidManager;
private:
	void init(const std::string& op_name, int64_t op_parentID, const std::string& op_uuid, bool op_completed, int64_t op_ID,
			time_t op_lastChange, const std::string& op_parentUuid, bool op_deleted);
};

extern bool operator== (const Task &op1, const Task &op2);
extern bool operator!= (const Task &op1, const Task &op2);
}
#endif /* TASK_H_ */
