/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"
#include "OSAbstraction.h"

namespace Test
{

TempDB::TempDB() : Database("/tmp/test.db")
{

}

TempDB::~TempDB()
{
	OSAbstraction::unlink("/tmp/test.db");
}

} /* namespace Test */
