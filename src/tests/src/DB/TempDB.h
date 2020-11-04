/*
 * TempDB.h
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#ifndef TEMPDB_H_
#define TEMPDB_H_

#include <Database.h>
#include <string>

class TempDB: public DB::Database
{
public:
	TempDB();
	virtual ~TempDB();
	void execute(std::string statement);
};

#endif /* TEMPDB_H_ */
