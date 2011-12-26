#include "CSQL.h"

#include <stdio.h>
#include <sstream>
#include <exception>

using namespace std;
namespace DBAbstraction
{

CSQL::CSQL()
{
}
CSQL::CSQL(const string& dbname)
{
	init(dbname);
}
void CSQL::init(const std::string& dbname)
{
	int rc = sqlite3_open(dbname.c_str(), &db);
	if (rc)
	{
		stringstream str;
		e.setReturnCode(rc);
		e.setMessage(sqlite3_errmsg(db));
		sqlite3_close(db);
		throw e;
	}
}

int64_t CSQL::getIDOfLastInsert()
{
	return sqlite3_last_insert_rowid(db);
}

void CSQL::exe(const std::string& s_exe)
{
	char *errorMessage;
	char **result;
	int rc;
	int nrow, ncol;
	const char* statement=s_exe.c_str();
	rc = sqlite3_get_table(db, /* An open database */
	statement, /* SQL to be executed */
	&result, /* Result written to a char *[]  that this points to */
	&nrow, /* Number of result rows written here */
	&ncol, /* Number of result columns written here */
	&errorMessage /* Error msg written here */
	);

	vcol_head.clear();
	rows.clear();

	if (rc == SQLITE_OK)
	{
		vector<DataCell> row;
		for (int c=0; c < ncol; c++)
		{
			vcol_head.push_back(result[c]);
		}
		for (int r=1; r<=nrow; r++)
		{
			row.clear();
			for (int c=0; c < ncol; c++)
			{
				DataCell cell(result[r*ncol+c]);
				row.push_back(cell);
			}
			rows.push_back(row);
		}
	}
	sqlite3_free_table(result);
	if (rc!=SQLITE_OK)
	{
		e.setReturnCode(rc);
		string message= "The statement ";
		message += statement;
		message += " caused :";
		message += errorMessage;
		e.setMessage(message);
		throw e;
	}
}

CSQL::~CSQL()
{
	if (db)
	{
		sqlite3_close(db);
	}
}
}

