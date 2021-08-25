#ifndef DB_UPDATER_H
#define DB_UPDATER_H

#include <libtimeit/db/database.h>

namespace libtimeit
{

class db_updater
{
public:
	db_updater(database& op_db) : db(op_db) {} ;
	void upgrade();
private:
	void upgrade_to_db_5();
	database &db;
	void update_times_to_db_5();
	void update_tasks_to_db_5();
	void upgrade_to_db_6();
	void upgrade_tasks_to_db_6();
	void upgrade_times_to_db_6();
};

}

#endif //DB_UPDATER_H
