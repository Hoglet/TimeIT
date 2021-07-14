/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "TempDB.h"

namespace test
{


TempDB::TempDB(notification_manager &notifier) : database(":memory:", notifier)
{
}



}