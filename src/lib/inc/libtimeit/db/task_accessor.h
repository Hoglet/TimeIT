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
	friend class SyncManager;

public:
	Task_accessor(Database& database);
	~Task_accessor();

	optional<Task> by_ID(int64_t taskID);
	vector<Task>   by_parent_ID(int64_t parent = 0);
	vector<Task>   changed_since(time_t timestamp = 0);

	Task_ID        create(const Task &task);
	bool           update(const Task &task);
	void           remove(int64_t taskID);

	Task_ID  ID(UUID uuid);
	void     enable_notifications(bool);

	void     setParentID(int64_t taskID, int parentID);
	void     setTaskExpanded(int64_t taskID, bool expanded);

protected:
	void      create_table()   override;
	void      create_views()   override {};
	void      upgrade()        override;
	void      upgrade_to_DB5();

	Database& database;

private:
	void notify(const Task &old_task, const Task &new_task);
	void _update(const Task &task);

	optional<class UUID> uuid(int64_t id);
	optional<Task>       get_task_unlimited(int64_t taskID);
};

}
#endif /* TASK_ACCESSOR_H_ */
