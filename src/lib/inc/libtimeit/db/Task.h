#ifndef TIMEIT_TASK_H
#define TIMEIT_TASK_H

#include <string>
#include "UUID.h"
#include <optional>
namespace Test
{
class TestTaskC;
class Network_test;
}
;

namespace libtimeit
{
class TaskAccessor;
using std::string;
using std::optional;
class Task
{
	friend class TaskAccessor;
	friend class SyncManager;

public:
	Task(const string& name, int64_t parentID = 0);
	Task(
		const string&  name,
		int64_t        parentID,
		UUID           uuid,
		bool           completed,
		int64_t        ID,
		time_t         lastChange,
		optional<class UUID> parentUUID,
		bool           deleted);

	virtual ~Task();

	string         name()        const;
	int64_t        ID()          const;
	int64_t        parentID()    const;
	bool           completed()   const;
	UUID           getUUID()        const;
	optional<UUID> parentUUID()  const;
	time_t         lastChanged() const;
	bool           deleted()     const;

	Task       withName(const string& newName) const;
	Task       withParent(int64_t)             const;
	Task       withCompleted(bool)             const;
	Task       withDeleted(bool)               const;

	friend bool       operator==(const Task &op1, const Task &op2);
	friend bool       operator!=(const Task &op1, const Task &op2);

protected:
	Task();

	int64_t              ID_          {0};
	string               name_;
	class UUID           uuid_;
	optional<class UUID> parentUuid_;
	bool                 completed_   {false};
	bool                 deleted_     {false};
	int64_t              parentID_    {0};
	time_t               lastChanged_ {0};

private:
	void operator=(const Task&) {};
};

extern bool operator==(const Task &op1, const Task &op2);
extern bool operator!=(const Task &op1, const Task &op2);
}

#endif