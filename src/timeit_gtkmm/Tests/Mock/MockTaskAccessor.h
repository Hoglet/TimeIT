#ifndef MOCKTASKACCESSOR_H_
#define MOCKTASKACCESSOR_H_

#include <libtimeit/db/ExtendedTaskAccessor.h>

namespace Test
{
using namespace libtimeit;
using namespace std;

class MockTaskAccessor: public IExtendedTaskAccessor
{
public:
	MockTaskAccessor();
	virtual ~MockTaskAccessor();
	virtual void attach(EventObserver*);
	virtual void detach(EventObserver*);

	virtual std::shared_ptr<Task> getTask(int64_t taskID);
	virtual std::shared_ptr<std::vector<Task>> getTasks(int64_t parentID = 0) ;
	virtual std::shared_ptr<std::vector<Task>> getTasksChangedSince(time_t timestamp = 0) ;
	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTask(int64_t taskID, time_t start = 0, time_t stop = 0, bool calculateTotalTime=true);
	virtual std::shared_ptr<std::vector<ExtendedTask>> getExtendedTasks(int64_t parentID = 0, time_t start = 0, time_t stop = 0);
	virtual std::shared_ptr<std::vector<ExtendedTask>> getRunningTasks(int64_t parentID = 0);
	virtual int64_t newTask(std::string name, int64_t parentID);
	virtual int64_t newTask(const Task& task);
	virtual bool updateTask(const Task& task);
	virtual void setTaskExpanded(int64_t taskID, bool expanded);
	virtual void setTaskName(int64_t taskID, std::string name);
	virtual void setParentID(int64_t taskID, int parentID);
	virtual void removeTask(int64_t taskID);
	virtual int64_t uuidToId(UUID uuid);
	virtual void enableNotifications(bool);
};

} /* namespace Test */
#endif /* MOCKTASKACCESSOR_H_ */
