#ifndef TASK_ACCESSOR_H_
#define TASK_ACCESSOR_H_

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task.h>

#include <libtimeit/db/notifier.h>
#include <string>


namespace libtimeit
{

class task_accessor
{
	friend class database;
public:
	task_accessor(database& database);

	optional<task> by_id(int64_t taskID);
	vector<task>   by_parent_id(int64_t parent = 0);
	vector<task>   changed_since(time_t timestamp = 0);

	Task_id        create(const task &item);
	bool           update(const task &item);
	void           remove(int64_t taskID);

	Task_id  id(UUID uuid);
	void     enable_notifications(bool);

	void     set_parent_id(Task_id task_id, Task_id parent_id);
	void     set_task_expanded(Task_id taskID, bool expanded);

protected:
	database& db; // NOLINT

private:
	void notify(const task &old_task, const task &item);
	void internal_update(const task &item);

	optional<class UUID> uuid(Task_id id);
	optional<task>       get_task_unlimited(Task_id taskID);

	sql_statement statement_uuid_to_id;
	sql_statement statement_get_task;
	sql_statement statement_id_to_uuid;
	sql_statement statement_new_task;

	static void  setup(database& db);
	static void  create_table(database& db);
	static void  upgrade(database& db);
	static void  upgrade_to_db_5(database& db);
	static void  internal_create(const task &item, sql_statement &statement_new_task);
};

}
#endif /* TASK_ACCESSOR_H_ */
