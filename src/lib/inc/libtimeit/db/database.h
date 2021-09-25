#ifndef DATABASE_H
#define DATABASE_H

#include "notifier.h"
#include <memory>
#include <libtimeit/db/sqlite3.h>
#include <libtimeit/db/query_result.h>


namespace libtimeit
{
class SQLite3;
using namespace std;

class database
{
public:
	database(const string& dbname, notification_manager& publisher);
	database() = delete;
	database(const database&) = delete;
	database(const database&&) = delete;
	~database() = default;
	void operator=(const database&) = delete;
	void operator=(const database&&) = delete;



	void begin_transaction();
	void try_rollback();
	void end_transaction();

	void enable_notifications(bool);
	void send_notification( message_type type, int64_t id, string name = "");

	query_result        execute( const string& statement);
	sql_statement           prepare(string statement);
	int64_t             id_of_last_insert();
	bool                table_exists(string name);
	[[nodiscard]] int   current_db_version() const;
	[[nodiscard]] bool  column_exists( string_view table, string_view column);
	void broadcast(function<void(event_observer*)> func) const;
private:
	int        db_version{0};
	notification_manager&  notifier;
	SQLite3    db;

	void find_db_version();
};

}

#endif /* DATABASE_H */
