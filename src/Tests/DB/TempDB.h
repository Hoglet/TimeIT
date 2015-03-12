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

namespace Test
{

class TempDB: public DB::Database
{
public:
	TempDB();
	virtual ~TempDB();
	void execute(std::string statement);
};

} /* namespace Test */
#endif /* TEMPDB_H_ */
