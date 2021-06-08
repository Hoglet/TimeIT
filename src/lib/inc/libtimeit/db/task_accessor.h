#ifndef TASK_ACCESSOR_H_
#define TASK_ACCESSOR_H_

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task.h>

#include <libtimeit/db/notifier.h>
#include <string>


namespace libtimeit
{

class Task_accessor
{
	friend class Database;
public:
	Task_accessor(Database& database);

	optional<Task> by_id(int64_t taskID);
	vector<Task>   by_parent_id(int64_t parent = 0);
	vector<Task>   changed_since(time_t timestamp = 0);

	Task_id        create(const Task &task);
	bool           update(const Task &task);
	void           remove(int64_t taskID);

	Task_id  id(UUID uuid);
	void     enable_notifications(bool);

	void     set_parent_id(Task_id task_id, Task_id parent_id);
	void     set_task_expanded(Task_id taskID, bool expanded);

protected:
	Database& database; // NOLINT

private:
	void notify(const Task &old_task, const Task &new_task);
	void internal_update(const Task &task);

	optional<class UUID> uuid(Task_id id);
	optional<Task>       get_task_unlimited(Task_id taskID);

	Statement statement_uuid_to_id;
	Statement statement_get_task;
	Statement statement_id_to_uuid;
	Statement statement_new_task;

	static void  setup(Database& database);
	static void  create_table(Database& db);
	static void  upgrade(Database& db);
	static void  upgrade_to_db_5(Database& db);
	static void  internal_create(const Task &task, Statement &statement_new_task);
};

}
#endif /* TASK_ACCESSOR_H_ */
