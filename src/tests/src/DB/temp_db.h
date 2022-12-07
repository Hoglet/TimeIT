/*
 * TempDB.h
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#ifndef TEMP_DB_H
#define TEMP_DB_H

#include <libtimeit/db/database.h>
#include <string>

namespace test
{
using namespace std;
using namespace libtimeit;

class temp_db : public libtimeit::database
{
public:
	temp_db(notification_manager &notifier);
};

}

#endif
