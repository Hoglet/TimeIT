#ifndef DATABASE_H_
#define DATABASE_H_

#include "Notifier.h"
#include <memory>
#include <libtimeit/db/CSQL.h>
#include <libtimeit/db/QueryResult.h>


namespace libtimeit
{
class CSQL;

class Database
{
public:
	Database(const std::string& dbname, Notifier& notifier);
	virtual ~Database();
	bool isThreadSafe();
	void beginTransaction();
	void tryRollback();
	void endTransaction();
	void enableNotifications(bool);
	void sendNotification(MessageType type, int64_t taskId);
	void attach(EventObserver *observer);
	void detach(EventObserver *observer);
	QueryResult exe(string statement);
	Statement prepare(string statement);
	int64_t getIDOfLastInsert();
protected:
	Notifier& m_notifier;
	CSQL db;

	void createTablesPartOne();
	void createAccessors();
	int getCurrentDBVersion();
	void upgradeIfNeeded(int DBVersion, const int expectedDBVersion);
	void createViews();
};

}

#endif /* DATABASE_H_ */
