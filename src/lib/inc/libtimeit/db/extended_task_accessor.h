#ifndef EXTENDED_TASK_ACCESSOR_H
#define EXTENDED_TASK_ACCESSOR_H

#include <vector>
#include <list>
#include <memory>
#include <string>

#include "extended_task.h"
#include "sqlite3.h"
#include "notifier.h"
#include "time_accessor.h"
#include "task_accessor.h"

namespace libtimeit
{
using namespace std;
class notification_manager;

class extended_task_accessor : public virtual task_accessor
{
public:
	friend class database;
	friend class time_accessor;
	extended_task_accessor(database& db);

	optional<extended_task> by_id(const task_id& id);
	//optional<extended_task> by_id(const task_id& id, time_t start = 0, time_t stop = 0);
	vector<extended_task>   by_parent_id(optional<task_id> id);

	void     set_task_expanded(const task_id& id, bool expanded);

private:
	static void setup(database& db);
	static void create_table(database& /*db*/)  {};
	static void drop_views(database& db);
	static void create_views(database& db);
	static void upgrade(database& db);

	vector<extended_task> get_extended_tasks(task_id id, optional<task_id> parent_id = {}, time_t start = 0, time_t stop = 0);
	time_accessor times;

	extended_task from_task( const task &item, vector <task_id> task_ids );
	vector<extended_task> from_tasks(const vector<task>& items);
	bool is_expanded(const task& task);

	sql_statement expanded_statement;
};

}
#endif // EXTENDED_TASK_ACCESSOR_H
