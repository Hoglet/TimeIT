/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"

namespace Test
{

TempDB::TempDB() : Database(":memory:")
{
	sqlite3_initialize();

}

TempDB::~TempDB()
{
	sqlite3_shutdown();
}

} /* namespace Test */
