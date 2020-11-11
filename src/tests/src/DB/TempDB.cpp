/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"

TempDB::TempDB() :
		Database(":memory:")
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

