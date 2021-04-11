#ifndef TIMEIT_SQLITE_H_
#define TIMEIT_SQLITE_H_

#include <libtimeit/exceptions/db_exception.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/query_result.h>
#include <stdint.h>
#include <memory>

namespace libtimeit
{

using namespace std;
class SQLite3;

class Statement
{
public:
	Statement(sqlite3_stmt* stmt, SQLite3& db);
	~Statement();
	void bind_value(int index, int64_t value);
	void bind_value(int index, const string& text);
	void bind_null_value(int index);
	Query_result execute();
private:
	SQLite3&      db;
	sqlite3_stmt* stmt;
	int           number_of_columns;
};

class SQLite3
{
public:
	SQLite3(string database_name);
	virtual ~SQLite3();
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
	SQLite3() {};
	SQLite3(const SQLite3&);
	void operator=(const SQLite3&);

	sqlite3*  db             {nullptr};
	bool      in_transaction {false};
};

}
#endif /*TIMEIT_SQLITE_H_*/
