#ifndef TASK_ACCESSOR_H_
#define TASK_ACCESSOR_H_

#include <libtimeit/db/accessor.h>
#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task.h>

#include <libtimeit/db/notifier.h>
#include <string>


namespace libtimeit
{

class Task_accessor: public Accessor
{
	friend class Database;
	friend class Sync_manager;

public:
	Task_accessor(Database& database);

	optional<Task> by_ID(int64_t taskID);
	vector<Task>   by_parent_ID(int64_t parent = 0);
	vector<Task>   changed_since(time_t timestamp = 0);

	Task_id        create(const Task &task);
	bool           update(const Task &task);
	void           remove(int64_t taskID);

	Task_id  ID(UUID uuid);
	void     enable_notifications(bool);

	void     set_parent_id(Task_id task_id, Task_id parent_id);
	void     set_task_expanded(Task_id taskID, bool expanded);

protected:
	void      create_table()   override;
	void      drop_views()     override {};
	void      create_views()   override {};
	void      upgrade()        override;
	void      upgrade_to_DB5();

	Database& database; // NOLINT

private:
	void notify(const Task &old_task, const Task &new_task);
	void _update(const Task &task);

	optional<class UUID> uuid(Task_id id);
	optional<Task>       get_task_unlimited(Task_id taskID);
};

}
#endif /* TASK_ACCESSOR_H_ */
