
#include <sstream>
#include <string>
#include <string.h>
#include <libtimeit/db/CSQL.h>
#include <cstring>

using namespace std;
namespace libtimeit
{

CSQL::CSQL(const string& dbname)
{
	db = nullptr;
	threadsafe=false;
	if (sqlite3_config(SQLITE_CONFIG_SERIALIZED) == SQLITE_OK)
	{
		threadsafe=true;
	}
	init(dbname);
}

CSQL::~CSQL()
{
	if (db)
	{
		sqlite3_close(db);
	}
}

void CSQL::init(const std::string& dbname)
{
	inTransaction = false;
	int rc = sqlite3_open(dbname.c_str(), &db);
	if (rc)
	{
		e.setReturnCode(rc);
		e.setMessage(sqlite3_errmsg(db));
		sqlite3_close(db);
		throw e;
	}
}

bool CSQL::isThreadSafe()
{
	return threadsafe;
}

int64_t CSQL::getIDOfLastInsert()
{
	return sqlite3_last_insert_rowid(db);
}

Statement CSQL::prepare(const char *buffer)
{
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		e.setReturnCode(rc);
		std::stringstream message;
		message << " (" << sqlite3_errmsg(db) << ")";
		message << "In statement: ";
		message << buffer;
		e.setMessage(message.str());
		tryRollback();
		throw e;
	}
	std::shared_ptr<CSQL> sharedMe = shared_from_this();
	return Statement(stmt, sharedMe);
}

QueryResult CSQL::exe(const std::string& s_exe)
{
	Statement statement = prepare(s_exe.c_str());
	return statement.execute();
}

void CSQL::beginTransaction()
{
	if (inTransaction)
	{
		e.setMessage("Already in transaction");
		tryRollback();
		throw e;
	}
	exe("BEGIN TRANSACTION");
	inTransaction = true;
}

void CSQL::endTransaction()
{
	if (inTransaction == false)
	{
		e.setMessage("No transaction in progress");
		throw e;
	}
	exe("COMMIT");
	inTransaction = false;
}
void CSQL::tryRollback()
{
	if (inTransaction)
	{
		exe("ROLLBACK");
		inTransaction = false;
	}
}

std::string CSQL::getLastErrorMessage()
{
	return sqlite3_errmsg(db);
}

//LCOV_EXCL_START
Statement::Statement()
{
	stmt = nullptr;
	ncol = 0;
}
//LCOV_EXCL_STOP

Statement::~Statement()
{
	sqlite3_finalize(stmt);
}

Statement::Statement(sqlite3_stmt* op_stmt, std::shared_ptr<CSQL> op_db): db(op_db)
{
	stmt = op_stmt;
	ncol = sqlite3_column_count(stmt);
}

void Statement::bindValue(int index, int64_t value)
{
	sqlite3_bind_int(stmt, index, value);
}

void Statement::bindValue(int index, const std::string& text)
{
	sqlite3_bind_text(stmt, index, text.c_str(), -1, SQLITE_TRANSIENT);
}

void Statement::bindNullValue(int index)
{
	sqlite3_bind_null(stmt, index);
}

QueryResult Statement::execute()
{
	QueryResult rows;
	while (true)
	{
		int rc = sqlite3_step(stmt);

		if (rc == SQLITE_DONE)
		{
			break;
		}
		else if (rc == SQLITE_ROW)
		{
			vector<DataCell> row;
			for (int c = 0; c < ncol; c++)
			{
				int type = sqlite3_column_type(stmt, c);
				if (type == SQLITE_INTEGER)
				{
					DataCell icell(sqlite3_column_int(stmt, c));
					row.push_back(icell);
				}
				else if (type == SQLITE_TEXT)
				{
					DataCell tcell((const char*) sqlite3_column_text(stmt, c));
					row.push_back(tcell);
				}
				else if (type == SQLITE_NULL)
				{
					DataCell tcell;
					row.push_back(tcell);
				}
				else
				{
					//LCOV_EXCL_START
					e.setMessage("Unmanaged data type");
					db->tryRollback();
					throw e;
					//LCOV_EXCL_STOP
				}
			}
			rows.push_back(row);
		}
		else
		{
			//LCOV_EXCL_START
			e.setReturnCode(rc);
			stringstream message;
			message << " (" << db->getLastErrorMessage() << ")";
			message << "Statement failed";
			e.setMessage(message.str());
			db->tryRollback();
			throw e;
			//LCOV_EXCL_STOP
		}
	}
	sqlite3_reset(stmt);
	return rows;
}

}

