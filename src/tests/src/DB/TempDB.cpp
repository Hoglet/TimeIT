/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"

namespace test
{


TempDB::TempDB(Notifier &notifier) : Database(":memory:", notifier)
{
	sqlite3_initialize();

}

TempDB::~TempDB()
{
	sqlite3_shutdown();
}

void TempDB::execute(string statement)
{
	db.execute(statement);
}

}