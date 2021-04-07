
#include <sstream>
#include <string>
#include <string.h>
#include <libtimeit/db/CSQL.h>

using namespace std;
namespace libtimeit
{

CSQL::CSQL(string dbname)
{
	init(dbname);
}

CSQL::~CSQL()
{
	sqlite3_close(db);
}

void CSQL::init(string dbname)
{
	in_transaction = false;
	int rc = sqlite3_open(dbname.c_str(), &db);
	if (rc)
	{
		auto message = sqlite3_errmsg(db);
		sqlite3_close(db);
		throw db_exception(message, rc);
	}
}

int64_t CSQL::ID_of_last_insert()
{
	return sqlite3_last_insert_rowid(db);
}

Statement CSQL::prepare(const string& query)
{
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query.c_str(), strlen(query.c_str()), &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::stringstream message;
		message << " (" << sqlite3_errmsg(db) << ")";
		message << "In query: ";
		message << query;
		try_rollback();
		throw db_exception(message.str(), rc);
	}
	return Statement(stmt, *this);
}

Query_result CSQL::execute(const string& query)
{
	Statement statement = prepare(query.data());
	return statement.execute();
}

void CSQL::begin_transaction()
{
	if (in_transaction)
	{
		try_rollback();
		throw db_exception("Already in transaction");
	}
	execute("BEGIN TRANSACTION");
	in_transaction = true;
}

void CSQL::end_transaction()
{
	if (in_transaction == false)
	{
		throw db_exception("No transaction in progress");
	}
	execute("COMMIT");
	in_transaction = false;
}
void CSQL::try_rollback()
{
	if (in_transaction)
	{
		execute("ROLLBACK");
		in_transaction = false;
	}
}

string CSQL::last_error_message()
{
	return sqlite3_errmsg(db);
}


Statement::~Statement()
{
	sqlite3_finalize(stmt);
}

Statement::Statement(sqlite3_stmt* op_stmt, CSQL& op_db): db(op_db)
{
	stmt = op_stmt;
	ncol = sqlite3_column_count(stmt);
}

void Statement::bind_value(int index, int64_t value)
{
	sqlite3_bind_int(stmt, index, value);
}

void Statement::bind_value(int index, const std::string& text)
{
	sqlite3_bind_text(stmt, index, text.c_str(), -1, SQLITE_TRANSIENT);
}

void Statement::bind_null_value(int index)
{
	sqlite3_bind_null(stmt, index);
}

Query_result Statement::execute()
{
	Query_result rows;
	while (true)
	{
		int rc = sqlite3_step(stmt);

		if (rc == SQLITE_DONE)
		{
			break;
		}
		else if (rc == SQLITE_ROW)
		{
			vector<Data_cell> row;
			for (int c = 0; c < ncol; c++)
			{
				int type = sqlite3_column_type(stmt, c);
				if (type == SQLITE_INTEGER)
				{
					Data_cell icell(sqlite3_column_int(stmt, c));
					row.push_back(icell);
				}
				else if (type == SQLITE_TEXT)
				{
					Data_cell tcell((const char*) sqlite3_column_text(stmt, c));
					row.push_back(tcell);
				}
				else if (type == SQLITE_NULL)
				{
					Data_cell tcell;
					row.push_back(tcell);
				}
				else
				{
					//LCOV_EXCL_START
					db.try_rollback();
					throw db_exception("Unmanaged data type");
					//LCOV_EXCL_STOP
				}
			}
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

}

