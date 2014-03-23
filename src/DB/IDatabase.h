#ifndef IDATABASE_H_
#define IDATABASE_H_

#include "IAutotrackAccessor.h"
#include "TimeAccessor.h"
#include "ExtendedTaskAccessor.h"
#include "ISettingsAccessor.h"

#include <memory>

namespace DB
{

class IDatabase
{
public:
	virtual ~IDatabase();
	virtual std::shared_ptr<IAutotrackAccessor> getAutotrackAccessor() = 0;
	virtual std::shared_ptr<ITimeAccessor> getTimeAccessor() = 0;
	virtual std::shared_ptr<IExtendedTaskAccessor> getExtendedTaskAccessor() = 0;
	virtual std::shared_ptr<ITaskAccessor> getTaskAccessor() = 0;
	virtual std::shared_ptr<ISettingsAccessor> getSettingsAccessor() = 0;
	virtual void beginTransaction() = 0;
	virtual void endTransaction() = 0;
};

}
#endif
