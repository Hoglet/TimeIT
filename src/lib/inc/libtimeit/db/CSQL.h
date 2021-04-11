#ifndef TIMEIT_CSQL_H_
#define TIMEIT_CSQL_H_

#include <libtimeit/exceptions/db_exception.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/query_result.h>
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
	void bind_value(int index, int64_t value);
	void bind_value(int index, const string& text);
	void bind_null_value(int index);
	Query_result execute();
private:
	CSQL&         db;
	sqlite3_stmt* stmt;
	int           ncol;
};

class CSQL
{
public:
	CSQL(string database_name);
	virtual ~CSQL();
	Query_result execute(const string& statement);
	int64_t      ID_of_last_insert();
	Statement    prepare(const string& query);
	string       last_error_message();
	void         begin_transaction();
	void         end_transaction();
	void         try_rollback();
protected:
	void         init(string database_name);
private:
	CSQL() {};
	CSQL(const CSQL&);
	void operator=(const CSQL&);

	sqlite3 *db             = nullptr;
	bool     in_transaction = false;
};

}
#endif /*CSQL_H_*/
