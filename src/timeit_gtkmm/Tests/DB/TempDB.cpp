/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include <libtimeit/db/Notifier.h>
#include "TempDB.h"

namespace Test
{

TempDB::TempDB(libtimeit::Notifier& op_notifier) : Database(":memory:", op_notifier)
{
	sqlite3_initialize();

}

TempDB::~TempDB()
{
	sqlite3_shutdown();
}

void TempDB::execute(std::string statement)
{
	db->exe(statement);
}

} /* namespace Test */
