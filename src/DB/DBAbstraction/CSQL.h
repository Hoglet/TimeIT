#ifndef CSQL_H_
#define CSQL_H_
#include "dbexception.h"
#include <sqlite3.h>
#include <string>
#include <vector>
#include "DataCell.h"
#include <stdint.h>


namespace DBAbstraction
{

class CSQL
{
public:
	CSQL(const std::string& databasename);
	void exe(const std::string& statement);
	virtual ~CSQL();
	std::vector<std::string> vcol_head;
	std::vector< std::vector<DataCell> > rows;
	int64_t getIDOfLastInsert();
protected:
	CSQL();
	void init(const std::string& databasename);
private:
	sqlite3 *db;
	dbexception e;
};
}
#endif /*CSQL_H_*/
