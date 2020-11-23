#ifndef TIMEIT_TASK_ACCESSOR_HPP_
#define TIMEIT_TASK_ACCESSOR_HPP_

#include <vector>
#include <list>
#include <memory>
#include <string>

#include "ExtendedTask.h"
#include "CSQL.h"
#include "Notifier.h"
#include "TimeAccessor.h"
#include "TaskAccessor.h"

namespace libtimeit
{

class Notifier;

class IExtendedTaskAccessor: public virtual ITaskAccessor
{
public:
	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTask(int64_t taskID, time_t start = 0, time_t stop = 0, bool calculateTotalTime=true) = 0;
	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0) = 0;
	virtual std::shared_ptr<std::vector<ExtendedTask>> getRunningTasks(int64_t parentID = 0) = 0;
};


class ExtendedTaskAccessor : public virtual TaskAccessor, public virtual IExtendedTaskAccessor
{
public:
	friend class TimeAccessor;
	ExtendedTaskAccessor(std::shared_ptr<CSQL>& op_db, Notifier& notifier, std::shared_ptr<ITimeAccessor> timeAccessor);
	virtual ~ExtendedTaskAccessor();

	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
	virtual std::shared_ptr<std::vector<ExtendedTask>> getRunningTasks(int64_t parentID = 0);
	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTask(int64_t taskID, time_t start = 0, time_t stop = 0, bool calculateTotalTime=true );
private:
	std::shared_ptr<std::vector<ExtendedTask>> _getExtendedTasks(int64_t taskID, int64_t parentID = 0, bool onlyRunning = false, time_t start = 0, time_t stop = 0);
	int getTotalChildTime(int64_t id, time_t start = 0 , time_t stop = 0);
	std::shared_ptr<ITimeAccessor> timeAccessor;
};

}
#endif // TIMEIT_TASK_MANAGER_HPP_
