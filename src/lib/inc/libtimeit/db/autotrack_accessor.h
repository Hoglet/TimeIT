/*
 * Auto_track_accessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef AUTO_TRACK_ACCESSOR_H_
#define AUTO_TRACK_ACCESSOR_H_
#include <vector>
#include "extended_task_accessor.h"
#include "autotrack_accessor.h"
#include "sqlite3.h"
#include <memory>

namespace libtimeit
{

using namespace std;

class Auto_track_accessor
{
	friend class Database;
public:
	Auto_track_accessor(Database& db);

	Task_id_list     task_ids(unsigned workspace);
	vector<unsigned> workspaces(int64_t task_ID);
	void             set_workspaces(int64_t task_ID, vector<unsigned> workspaces);
private:
	Extended_task_accessor task_accessor;
	Database&              database;
	static void setup(Database& database);
	static void create_table(Database& db);
	static void upgrade(Database& db);
};

}
#endif /* AUTO_TRACK_ACCESSOR_H_ */
