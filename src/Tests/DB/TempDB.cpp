/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"

namespace Test
{

TempDB::TempDB() : Database("/tmp/test.db")
{

}

TempDB::~TempDB()
{
	unlink("/tmp/test.db");
}

} /* namespace Test */
