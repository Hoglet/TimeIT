#pragma once

#include <string>

namespace Test
{
class TestTaskC;
class Network_test;
}
;

namespace DB
{
class TaskAccessor;
using std::string;

class Task
{
	friend class TaskAccessor;
	friend class SyncManager;

public:
	Task(const string& name, int64_t parentID = 0);
	Task(
		const string&  name,
		int64_t        parentID,
		const string&  uuid,
		bool           completed,
		int64_t        ID,
		time_t         lastChange,
		const string&  parentUUID,
		bool           deleted);

	virtual ~Task();

	string   name()        const;
	int64_t  ID()          const;
	int64_t  parentID()    const;
	bool     completed()   const;
	string   UUID()        const;
	string   parentUUID()  const;
	time_t   lastChanged() const;
	bool     deleted()     const;

	Task     withName(const string& newName) const;
	Task     withParent(int64_t)             const;
	Task     withCompleted(bool)             const;
	Task     withDeleted(bool)               const;

	friend bool       operator==(const Task &op1, const Task &op2);
	friend bool       operator!=(const Task &op1, const Task &op2);

protected:
	Task();

	int64_t  ID_          {0};
	string   name_;
	string   uuid_;
	string   parentUuid_;
	bool     completed_   {false};
	bool     deleted_     {false};
	int64_t  parentID_    {0};
	time_t   lastChanged_ {0};

private:
	void operator=(const Task&) {};
};

extern bool operator==(const Task &op1, const Task &op2);
extern bool operator!=(const Task &op1, const Task &op2);
}
