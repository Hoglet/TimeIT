#ifndef DATABASE_H_
#define DATABASE_H_

#include "notifier.h"
#include <memory>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/query_result.h>
#include <libtimeit/db/accessor.h>


namespace libtimeit
{
class SQLite3;
using namespace std;

class Database
{
public:
	Database(const string& dbname, Notifier& publisher);
	virtual ~Database();

	void begin_transaction();
	void try_rollback();
	void end_transaction();

	void enable_notifications(bool);
	void send_notification(message_type type, int64_t taskId, string name = "");

	Query_result execute(const string& statement);
	Statement    prepare(string statement);
	int64_t      ID_of_last_insert();
	bool         table_exists(string name);
	int          current_DB_version();
	bool         column_exists( string_view string, string_view string_1);
protected:
	Notifier& notifier;
	SQLite3      db;

	void create_tables( list<Accessor*>& accessors );
	void upgrade( list<Accessor*>&  accessors);
	void drop_views( list<Accessor*>& accessors );
	void create_views( list<Accessor*>& accessors );
private:
	Database(const Database&);
	void operator=(const Database&);
	int db_version{0};

	void find_db_version();
};

}

#endif /* DATABASE_H_ */
