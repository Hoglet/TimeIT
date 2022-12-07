/*
 * Auto_track_accessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef AUTOTRACK_ACCESSOR_H
#define AUTOTRACK_ACCESSOR_H
#include <vector>
#include "extended_task_accessor.h"
#include "autotrack_accessor.h"
#include "sqlite.h"
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
	vector<unsigned> workspaces(const task_id& id);
	void             set_workspaces(const task_id& id, const vector<unsigned>& workspaces);
private:
	extended_task_accessor tasks;
	database&              db;
	static void setup(database& db);
	static void create_table(database& db);
	static void upgrade(database& db);
};

}
#endif /* AUTOTRACK_ACCESSOR_H */
