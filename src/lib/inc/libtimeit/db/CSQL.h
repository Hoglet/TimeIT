#ifndef TIMEIT_CSQL_H_
#define TIMEIT_CSQL_H_

#include <libtimeit/exceptions/dbexception.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/QueryResult.h>
#include <stdint.h>
#include <memory>

namespace libtimeit
{

using namespace std;
class CSQL;

class Statement
{
public:
	Statement(sqlite3_stmt* stmt, CSQL& db);
	~Statement();
	void bindValue(int index, int64_t value);
	void bindValue(int index, const string& text);
	void bindNullValue(int index);
	QueryResult execute();
private:
	CSQL& db;
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
	sqlite3 *db = nullptr;
	dbexception e;
	bool inTransaction = false;
	bool threadsafe = false;
};

}
#endif /*CSQL_H_*/
