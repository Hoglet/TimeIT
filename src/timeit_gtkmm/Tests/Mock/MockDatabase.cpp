#include "MockDatabase.h"
#include "MockAutotrackAccessor.h"
#include "MockTaskAccessor.h"
#include "MockTimeAccessor.h"
#include "MockSettingsAccessor.h"

namespace Test
{

MockDatabase::MockDatabase()
{
}

MockDatabase::~MockDatabase()
{
}

std::shared_ptr<IAutotrackAccessor> MockDatabase::getAutotrackAccessor()
{
	return std::shared_ptr<IAutotrackAccessor>(new MockAutotrackAccessor);
}

std::shared_ptr<ITimeAccessor> MockDatabase::getTimeAccessor()
{
	return std::shared_ptr<ITimeAccessor>(new MockTimeAccessor);
}

std::shared_ptr<IExtendedTaskAccessor> MockDatabase::getExtendedTaskAccessor()
{
	return std::shared_ptr<IExtendedTaskAccessor>(new MockTaskAccessor);
}

std::shared_ptr<ISettingsAccessor> MockDatabase::getSettingsAccessor()
{
	return std::shared_ptr<ISettingsAccessor>(new MockSettingsAccessor);
}
std::shared_ptr<ITaskAccessor> MockDatabase::getTaskAccessor()
{
	return std::shared_ptr<ITaskAccessor>(new MockTaskAccessor);
}

bool MockDatabase::isThreadSafe()
{
	return true;
}
void MockDatabase::beginTransaction()
{

}
void MockDatabase::endTransaction()
{

}


} /* namespace Test */
