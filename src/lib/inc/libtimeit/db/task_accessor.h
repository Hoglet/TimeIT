#ifndef TASK_ACCESSOR_H_
#define TASK_ACCESSOR_H_

#include <libtimeit/db/data_types.h>
#include <libtimeit/db/database.h>
#include <libtimeit/datatypes/task.h>
#include <libtimeit/db/notifier.h>
#include <libtimeit/datatypes/task_id.h>
#include <string>


namespace libtimeit
{

class task_accessor
{
	friend class database;
public:
	task_accessor(database& database);

	optional<task> by_id(const task_id& id);
	vector<task>   by_parent_id(optional<task_id> parent_id = {});
	vector<task>   changed_since(time_t timestamp = 0);

	void           create(const task &item);
	bool           update(const task &item);
	void           remove(const task_id& id);

	Task_id  to_id(const task_id& id);
	Task_id  to_id(const UUID& uuid);
	void     enable_notifications(bool);

protected:
	database& db; // NOLINT

private:
	void notify(const task &old_task, const task &item);
	void internal_update(const task &item);

	optional<task_id>    optional_task_id(Task_id id);
	optional<class UUID> uuid(Task_id id);
	optional<task_id>    to_task_id(Task_id id);
	optional<task>       get_task_unlimited(const task_id& id);

	sql_statement statement_uuid_to_id;
	sql_statement statement_get_by_id;
	sql_statement statement_get_task;
	sql_statement statement_id_to_uuid;
	sql_statement statement_new_task;

	static void  setup( database& db );
	static void  create_table( database& db );
	void  internal_create( const task &item );
};

}
#endif /* TASK_ACCESSOR_H_ */
