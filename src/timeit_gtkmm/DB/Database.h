#ifndef DATABASE_H_
#define DATABASE_H_

#include "IDatabase.h"
#include <AutotrackAccessor.h>
#include "TimeAccessor.h"
#include <ExtendedTaskAccessor.h>
#include <ISettingsAccessor.h>
#include <Notifier.h>
#include <memory>

namespace DBAbstraction { class CSQL; }

namespace DB
{

class Database: public IDatabase
{
public:
	Database(const std::string& dbname);
	virtual ~Database();
	std::shared_ptr<IAutotrackAccessor> getAutotrackAccessor();
	std::shared_ptr<ITimeAccessor> getTimeAccessor();
	std::shared_ptr<ITaskAccessor> getTaskAccessor();
	std::shared_ptr<IExtendedTaskAccessor> getExtendedTaskAccessor();
	std::shared_ptr<ISettingsAccessor> getSettingsAccessor();
	bool isThreadSafe();
	void beginTransaction();
	void tryRollback();
	void endTransaction();
protected:
	std::shared_ptr<IAutotrackAccessor> m_autotrackAccessor;
	std::shared_ptr<TimeAccessor> m_timeAccessor;
	std::shared_ptr<IExtendedTaskAccessor> m_extendedTaskAccessor;
	std::shared_ptr<TaskAccessor> m_taskAccessor;
	std::shared_ptr<ISettingsAccessor> m_settingsAccessor;
	std::shared_ptr<Notifier> m_notifier;
	std::shared_ptr<DBAbstraction::CSQL> db;

	void createTablesPartOne();
	void createAccessors();
	int getCurrentDBVersion();
	void upgradeIfNeeded(int DBVersion, const int expectedDBVersion);
	void createViews();
};

}

#endif /* DATABASE_H_ */
