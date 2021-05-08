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
	Database() = delete;
	Database(const Database&) = delete;
	Database(const Database&&) = delete;
	~Database() = default;
	void operator=(const Database&) = delete;
	void operator=(const Database&&) = delete;



	void begin_transaction();
	void try_rollback();
	void end_transaction();

	void enable_notifications(bool);
	void send_notification(message_type type, int64_t taskId, string name = "");

	Query_result        execute(const string& statement);
	Statement           prepare(string statement);
	int64_t             ID_of_last_insert();
	bool                table_exists(string name);
	[[nodiscard]] int   current_DB_version() const;
	[[nodiscard]] bool  column_exists( string_view string, string_view string_1);
protected:
	static void create_tables( list<Accessor*>& accessors );
	void upgrade( list<Accessor*>&  accessors);
	static void drop_views( list<Accessor*>& accessors );
	static void create_views( list<Accessor*>& accessors );
private:
	int        db_version{0};
	Notifier&  notifier;
	SQLite3    db;

	void find_db_version();
};

}

#endif /* DATABASE_H_ */