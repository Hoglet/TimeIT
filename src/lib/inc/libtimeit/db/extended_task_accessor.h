#ifndef TIMEIT_TASK_ACCESSOR_HPP_
#define TIMEIT_TASK_ACCESSOR_HPP_

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
class Notifier;

class Extended_task_accessor : public virtual Task_accessor
{
public:
	friend class Database;
	friend class Time_accessor;
	Extended_task_accessor(Database& database);

	optional<Extended_task> by_id(int64_t taskID, time_t start = 0, time_t stop = 0);
	vector<Extended_task>   by_parent_id(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
private:
	static void setup(Database& db);
	static void create_table(Database& db)  {};
	static void drop_views(Database& db);
	static void create_views(Database& db);
	static void upgrade(Database& db);

	vector<Extended_task> get_extended_tasks(int64_t taskID, int64_t parentID = 0, bool onlyRunning = false, time_t start = 0, time_t stop = 0);
	Duration get_total_child_time(int64_t id, time_t start = 0, time_t stop = 0);
	Time_accessor time_accessor;

};

}
#endif // TIMEIT_TASK_MANAGER_HPP_
