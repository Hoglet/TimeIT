/*
 * Database.h
 *
 *  Created on: Mar 3, 2009
 *      Author: hoglet
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include "IDatabase.h"
#include <IAutotrackAccessor.h>
#include <ITimeAccessor.h>
#include <ITaskAccessor.h>
#include <ISettingsAccessor.h>
#include <Notifier.h>
#include <memory>

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
	std::shared_ptr<ISettingsAccessor> getSettingsAccessor();
private:
	std::shared_ptr<IAutotrackAccessor> m_autotrackAccessor;
	std::shared_ptr<ITimeAccessor> m_timeAccessor;
	std::shared_ptr<ITaskAccessor> m_taskAccessor;
	std::shared_ptr<ISettingsAccessor> m_settingsAccessor;
	std::shared_ptr<Notifier> m_notifier;
};

}

#endif /* DATABASE_H_ */
