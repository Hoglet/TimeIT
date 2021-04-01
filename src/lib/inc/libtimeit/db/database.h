#ifndef DATABASE_H_
#define DATABASE_H_

#include "notifier.h"
#include <memory>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/query_result.h>


namespace libtimeit
{
class CSQL;

class Database
{
public:
	Database(const std::string& dbname, Notifier& publisher);
	virtual ~Database();

	void begin_transaction();
	void try_rollback();
	void end_transaction();

	void enable_notifications(bool);
	void send_notification(message_type type, int64_t taskId, string name = "");

	Query_result execute(const string& statement);
	Statement    prepare(string statement);
	int64_t      ID_of_last_insert();
protected:
	Notifier& notifier;
	CSQL      db;

	void create_tables();
	int  current_DB_version();
	void upgrade_if_needed(int DB_version, const int expected_DB_version);
	void createViews();
private:
	Database();
	Database(const Database&);
	void operator=(const Database&);

};

}

#endif /* DATABASE_H_ */
