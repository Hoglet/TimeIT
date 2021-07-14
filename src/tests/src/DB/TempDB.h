/*
 * TempDB.h
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#ifndef TIMEIT_TEMPDB_H
#define TIMEIT_TEMPDB_H

#include <libtimeit/db/database.h>
#include <string>

namespace test
{
using namespace std;
using namespace libtimeit;

class TempDB : public libtimeit::database
{
public:
	TempDB(notification_manager &notifier);
};

}

#endif