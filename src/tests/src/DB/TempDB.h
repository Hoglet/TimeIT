/*
 * TempDB.h
 *
 *  Created on: 9 Oct 2012
 *      Author: hoglet
 */

#pragma once
#include <Database.h>
#include <string>

class TempDB: public DB::Database
{
public:
	TempDB();
	virtual ~TempDB();
	void execute(std::string statement);
};

