/*
 * Autotrack_accessor.h
 *
 *  Created on: 2008-jun-25
 *      Author: hoglet
 */

#ifndef AUTO_TRACK_ACCESSOR_H_
#define AUTO_TRACK_ACCESSOR_H_
#include <vector>
#include <libtimeit/db/accessor.h>
#include "extended_task_accessor.h"
#include "autotrack_accessor.h"
#include "sqlite3.h"
#include <memory>

namespace libtimeit
{

using namespace std;

class Auto_track_accessor: public Accessor
{
	friend class Database;
public:
	Auto_track_accessor(Database& db);
	~Auto_track_accessor() = default;

	Task_id_list     task_IDs(int workspace);
	vector<unsigned> workspaces(int64_t task_ID);
	void             set_workspaces(int64_t task_ID, vector<unsigned> workspaces);
protected:
	void      create_table()   override;
	void      drop_views()     override {};
	void      create_views()   override {};
	void      upgrade()        override;
private:
	Extended_task_accessor task_accessor;
	Database&              database;
};

}
#endif /* AUTO_TRACK_ACCESSOR_H_ */
