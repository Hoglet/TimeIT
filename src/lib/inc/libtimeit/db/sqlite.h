#ifndef SQLITE_H
#define SQLITE_H

#include <libtimeit/exceptions/db_exception.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <libtimeit/db/sqlite.h>
#include <libtimeit/db/query_result.h>
#include <cstdint>
#include <memory>
#include <chrono>

namespace libtimeit
{

using namespace std;
using namespace std::chrono;
class sqlite;

class sql_statement
{
public:
	sql_statement( sqlite3_stmt* stmt, sqlite& db);
	sql_statement(const sql_statement&) = delete;
	sql_statement(sql_statement&&) noexcept ;
	sql_statement& operator=(const sql_statement&) = delete;
	sql_statement& operator=(sql_statement&&) = delete;

	~sql_statement();
	void bind_value(int index, bool value);
	void bind_value(int index, int  value);
	void bind_value(int index, int64_t value);
	void bind_value(int index, const string& text);
	void bind_value(int index, time_point<system_clock> value);
	void bind_null_value(int index);

	query_result execute();
private:
	sqlite&      db;
	sqlite3_stmt* stmt;
	int           number_of_columns;

	vector<data_cell> get_row();
};

class sqlite
{
public:
	explicit sqlite( string database_name);
	sqlite() = delete;
	sqlite( const sqlite&) = delete;
	void operator=(const sqlite&) = delete;
	sqlite( sqlite&&) noexcept ;
	sqlite& operator=( sqlite&&) = delete;


	virtual ~sqlite();
	query_result execute( const string& statement);
	int64_t      id_of_last_insert();
	sql_statement    prepare(const string& query);
	string       last_error_message();
	void         begin_transaction();
	void         end_transaction();
	void         try_rollback();
protected:
	void         init(string database_name);
private:

	sqlite3*  db             {nullptr};
	bool      in_transaction {false};
};

}
#endif /*SQLITE_H*/
