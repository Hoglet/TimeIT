#ifndef LEGACY_DB_HELPER_H
#define LEGACY_DB_HELPER_H
#include <libtimeit/datatypes/task_id.h>
#include <libtimeit/db/database.h>


namespace legacy_db_helper
{
	using namespace libtimeit;
	using Task_id  = int64_t;

	Task_id           to_id(const task_id& id, database& db);
	Task_id           to_id(const uuid& id, database& db);
	Task_id           new_task_id_to_old(task_id owner_id, database& db);
	optional<task_id> old_task_id_to_new(int64_t owner_id, database& db);
}


#endif //LEGACY_DB_HELPER_H