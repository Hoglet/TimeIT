/*
 * MockDatabase.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKDATABASE_H_
#define MOCKDATABASE_H_

#include "IDatabase.h"

namespace Test
{
using namespace DB;

class MockDatabase: public IDatabase
{
public:
	MockDatabase();
	virtual ~MockDatabase();

	virtual std::shared_ptr<IAutotrackAccessor> getAutotrackAccessor();
	virtual std::shared_ptr<ITimeAccessor> getTimeAccessor();
	virtual std::shared_ptr<ITaskAccessor> getTaskAccessor();
	virtual std::shared_ptr<ISettingsAccessor> getSettingsAccessor();
};

} /* namespace Test */
#endif /* MOCKDATABASE_H_ */
