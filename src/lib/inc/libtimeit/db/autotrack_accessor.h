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

class auto_track_accessor
{
	friend class database;
public:
	auto_track_accessor(database& db);

	task_id_list     task_ids(unsigned workspace);
	vector<unsigned> workspaces(task_id id);
	void             set_workspaces(task_id task_ID, vector<unsigned> workspaces);
private:
	extended_task_accessor tasks;
	database&              db;
	static void setup(database& db);
	static void create_table(database& db);
	static void upgrade(database& db);
};

}
#endif /* AUTO_TRACK_ACCESSOR_H_ */
