
#include <sstream>
#include <string>
#include <cstring>
#include <libtimeit/db/sqlite3.h>

using namespace std;
namespace libtimeit
{

SQLite3::SQLite3(string dbname)
{
	init(dbname);
}
SQLite3::SQLite3(SQLite3&& other):
	db(std::move(other.db)),
	in_transaction(other.in_transaction)
{
}

SQLite3::~SQLite3()
{
	sqlite3_close(db);
}

void SQLite3::init(string dbname)
{
	in_transaction = false;
	int rc = sqlite3_open(dbname.c_str(), &db);
	if ( rc != SQLITE_OK )
	{
		const auto* message = sqlite3_errmsg(db);
		sqlite3_close(db);
		throw db_exception(message, rc);
	}
}

int64_t SQLite3::id_of_last_insert()
{
	return sqlite3_last_insert_rowid(db);
}

sql_statement SQLite3::prepare(const string& query)
{
	sqlite3_stmt* stmt{nullptr};
	int rc = sqlite3_prepare_v2(db, query.c_str(), (int)strlen(query.c_str()), &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::stringstream message;
		message << " (" << sqlite3_errmsg(db) << ")";
		message << "In query: ";
		message << query;
		try_rollback();
		throw db_exception(message.str(), rc);
	}
	return sql_statement(stmt, *this);
}

Query_result SQLite3::execute(const string& query)
{
	sql_statement statement = prepare(query);
	return statement.execute();
}

void SQLite3::begin_transaction()
{
	if (in_transaction)
	{
		try_rollback();
		throw db_exception("Already in transaction");
	}
	execute("BEGIN TRANSACTION");
	in_transaction = true;
}

void SQLite3::end_transaction()
{
	if (in_transaction )
	{
		execute("COMMIT");
		in_transaction = false;
	}
	else
	{
		throw db_exception("No transaction in progress");
	}
}
void SQLite3::try_rollback()
{
	if (in_transaction)
	{
		execute("ROLLBACK");
		in_transaction = false;
	}
}

string SQLite3::last_error_message()
{
	return sqlite3_errmsg(db);
}

sql_statement::sql_statement(sql_statement&& other):
		db(other.db),
		stmt(std::move(other.stmt)),
        number_of_columns(other.number_of_columns)
{
}

sql_statement::~sql_statement()
{
	sqlite3_finalize(stmt);
}

sql_statement::sql_statement(sqlite3_stmt* op_stmt, SQLite3& op_db):
	db(op_db),
	stmt(op_stmt),
	number_of_columns(sqlite3_column_count(stmt))
{

}

void sql_statement::bind_value(int index, int64_t value)
{
	sqlite3_bind_int(stmt, index, (int)value);
}

void sql_statement::bind_value(int index, const std::string& text)
{
	sqlite3_bind_text(stmt, index, text.c_str(), -1, SQLITE_TRANSIENT); // NOLINT
}

void sql_statement::bind_null_value(int index)
{
	sqlite3_bind_null(stmt, index);
}

Query_result sql_statement::execute()
{
	Query_result rows;
	while (true)
	{
		int rc = sqlite3_step(stmt);

		if (rc == SQLITE_DONE)
		{
			break;
		}
		if (rc == SQLITE_ROW)
		{
			vector<data_cell> row = get_row();
			rows.push_back(row);
		}
		else
		{
			//LCOV_EXCL_START
			stringstream message;
			message << " (" << db.last_error_message() << ")";
			message << "Statement failed";
			db.try_rollback();
			throw db_exception(message.str(), rc);
			//LCOV_EXCL_STOP
		}
	}
	sqlite3_reset(stmt);
	return rows;
}

vector<data_cell> sql_statement::get_row()
{
	vector<data_cell> row;
	for (int c = 0; c < number_of_columns; c++)
	{
		int type = sqlite3_column_type(stmt, c);
		if (type == SQLITE_INTEGER)
		{
			row.emplace_back(sqlite3_column_int(stmt, c));
		}
		else if (type == SQLITE_TEXT)
		{
			row.emplace_back((const char*) sqlite3_column_text(stmt, c)); // NOLINT
		}
		else if (type == SQLITE_NULL)
		{
			row.emplace_back();
		}
		else
		{
			//LCOV_EXCL_START
			db.try_rollback();
			throw db_exception("Unmanaged data type");
			//LCOV_EXCL_STOP
		}
	}
	return row;
}

}

