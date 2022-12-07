/*
 * TempDB.cpp
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#include "temp_db.h"

namespace test
{


temp_db::temp_db(notification_manager &notifier) : database(":memory:", notifier)
{
}



}
