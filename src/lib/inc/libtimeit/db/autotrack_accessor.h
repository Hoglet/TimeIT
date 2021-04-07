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
#include "CSQL.h"
#include <memory>

namespace libtimeit
{

using namespace std;

class Autotrack_accessor: public Accessor
{
	friend class Database;
public:
	Autotrack_accessor(Database& db);
	~Autotrack_accessor() = default;

	Task_ID_list    task_IDs(int workspace);
	vector<int>     workspaces(int64_t task_ID);
	void            set_workspaces(int64_t task_ID, vector<int> workspaces);
protected:
	void      create_table()   override;
	void      create_views()   override {};
	void      upgrade()        override;
private:
	Extended_task_accessor task_accessor;
	Database&              database;
};

}
#endif /* AUTO_TRACK_ACCESSOR_H_ */
