/*
 * Database.h
 *
 *  Created on: Mar 3, 2009
 *      Author: hoglet
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <IAutotrackAccessor.h>
#include <ITimeAccessor.h>
#include <ITaskAccessor.h>
#include <boost/shared_ptr.hpp>
#include <ISettingsAccessor.h>

namespace DB
{

class Database
{
public:
	Database(const std::string& dbname);
	virtual ~Database();
	boost::shared_ptr<IAutotrackAccessor> getAutotrackAccessor();
	boost::shared_ptr<ITimeAccessor> getTimeAccessor();
	boost::shared_ptr<ITaskAccessor> getTaskAccessor();
	boost::shared_ptr<ISettingsAccessor> getSettingsAccessor();
private:
	boost::shared_ptr<IAutotrackAccessor> m_autotrackAccessor;
	boost::shared_ptr<ITimeAccessor> m_timeAccessor;
	boost::shared_ptr<ITaskAccessor> m_taskAccessor;
	boost::shared_ptr<ISettingsAccessor> m_settingsAccessor;
};

}

#endif /* DATABASE_H_ */
