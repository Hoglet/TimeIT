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
using namespace std;
class Notifier;

class ExtendedTaskAccessor : public virtual TaskAccessor
{
public:
	friend class TimeAccessor;
	ExtendedTaskAccessor(Database& database);
	virtual ~ExtendedTaskAccessor();

	virtual vector<ExtendedTask> getExtendedTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
	virtual vector<ExtendedTask> getRunningTasks(int64_t parentID = 0);
	virtual vector<ExtendedTask> getExtendedTask(int64_t taskID, time_t start = 0, time_t stop = 0, bool calculateTotalTime=true );
private:
	vector<ExtendedTask> _getExtendedTasks(int64_t taskID, int64_t parentID = 0, bool onlyRunning = false, time_t start = 0, time_t stop = 0);
	int getTotalChildTime(int64_t id, time_t start = 0 , time_t stop = 0);
	TimeAccessor time_accessor;

};

}
#endif // TIMEIT_TASK_MANAGER_HPP_
