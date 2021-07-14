#ifndef EXTENDED_TASK_ACCESSOR_HPP_
#define EXTENDED_TASK_ACCESSOR_HPP_

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

	optional<extended_task> by_id(int64_t taskID, time_t start = 0, time_t stop = 0);
	vector<extended_task>   by_parent_id(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
private:
	static void setup(database& db);
	static void create_table(database& /*db*/)  {};
	static void drop_views(database& db);
	static void create_views(database& db);
	static void upgrade(database& db);

	vector<extended_task> get_extended_tasks(int64_t taskID, int64_t parentID = 0, bool onlyRunning = false, time_t start = 0, time_t stop = 0);
	Duration get_total_child_time(int64_t id, time_t start = 0, time_t stop = 0);
	time_accessor times;

};

}
#endif // EXTENDED_TASK_ACCESSOR_HPP_
