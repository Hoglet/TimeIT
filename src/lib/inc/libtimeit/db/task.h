#ifndef TIMEIT_TASK_H
#define TIMEIT_TASK_H

#include <string>
#include <optional>
#include <libtimeit/db/data_types.h>
#include <libtimeit/db/uuid.h>

namespace Test
{
class TestTaskC;
class Network_test;
}
;

namespace libtimeit
{
class Task_accessor;

using namespace std;

class Task
{
	friend class Task_accessor;
	friend class SyncManager;

public:
	Task(string name, Task_ID parent_ID = 0);
	Task(
		string         name,
		Task_ID        parent_ID,
		UUID           uuid,
		bool           completed,
		Task_ID        ID,
		time_t         last_change,
		optional<UUID> parent_UUID,
		bool           deleted);

	virtual ~Task();

	string         name()         const;
	Task_ID        ID()           const;
	Task_ID        parent_ID()    const;
	bool           completed()    const;
	UUID           get_UUID()     const;
	optional<UUID> parent_UUID()  const;
	time_t         last_changed() const;
	bool           deleted()      const;

	Task       with_name(string new_name) const;
	Task       with_parent(Task_ID)              const;
	Task       with_completed(bool)              const;
	Task       with_deleted(bool)                const;

	friend bool       operator==(const Task &op1, const Task &op2);
	friend bool       operator!=(const Task &op1, const Task &op2);

protected:
	Task();

	Task_ID              ID_          {0};
	string               name_;
	class UUID           uuid_;
	optional<class UUID> parent_Uuid_;
	bool                 completed_   {false};
	bool                 deleted_     {false};
	Task_ID              parent_ID_    {0};
	time_t               last_changed_ {0};

private:
	void operator=(const Task&) {};
};

extern bool operator==(const Task &op1, const Task &op2);
extern bool operator!=(const Task &op1, const Task &op2);
}

#endif