/*
 * TempDB.h
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#ifndef TEMPDB_H_
#define TEMPDB_H_

#include <libtimeit/db/Database.h>
#include <string>

namespace Test
{

class TempDB: public libtimeit::Database
{
public:
	TempDB(libtimeit::Notifier& op_notifier);
	virtual ~TempDB();
	void execute(std::string statement);
private:
	TempDB();

};

} /* namespace Test */
#endif /* TEMPDB_H_ */
