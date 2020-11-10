#ifndef CSQL_H_
#define CSQL_H_

#include "dbexception.h"
#include <sqlite3.h>
#include <string>
#include <vector>
#include "DataCell.h"
#include <stdint.h>
#include <memory>

namespace DBAbstraction
{

class CSQL;

typedef  std::vector<std::vector<DataCell>> QueryResult;

class Statement
{
public:
	Statement(sqlite3_stmt* stmt, std::shared_ptr<CSQL> db);
	~Statement();
	void bindValue(int index, int64_t value);
	void bindValue(int index, const std::string& text);
	void bindNullValue(int index);
	QueryResult execute();
private:
	Statement();
	std::shared_ptr<CSQL> db;
	sqlite3_stmt* stmt;
	dbexception e;
	int ncol;
};

class CSQL: public std::enable_shared_from_this<CSQL>
{
public:
	CSQL(const std::string& databasename);
	virtual ~CSQL();
	QueryResult exe(const std::string& statement);
	int64_t getIDOfLastInsert();
	Statement prepare(const char *);
	void beginTransaction();
	void endTransaction();
	void tryRollback();
	std::string getLastErrorMessage();
	bool isThreadSafe();
protected:
	void init(const std::string& databasename);
private:
	CSQL();
	sqlite3 *db;
	dbexception e;
	bool inTransaction;
	bool threadsafe;
};

}
#endif /*CSQL_H_*/
