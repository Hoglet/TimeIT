#ifndef SYNCMANAGER_H_
#define SYNCMANAGER_H_

#include <memory>
#include "Database.h"
#include "Timer.h"
#include "Network.h"
#include "Json.h"

namespace syncing
{
class SyncManager
{
public:
	SyncManager(std::shared_ptr<DB::IDatabase>&, std::shared_ptr<INetwork>& network);
	virtual ~SyncManager();
	void start();
	void stop();
	void completeSync();
private:
	SyncManager();
	bool isActive();
	void worker();
	int syncTasks();
	int syncTimes();
	int syncTimesToDatabase(std::string result);
	int syncTaskToDatabase(std::string result);

	std::shared_ptr<DB::IDatabase> db;
	std::shared_ptr<DB::ITaskAccessor> taskAccessor;
	std::shared_ptr<DB::ITimeAccessor> timeAccessor;
	std::shared_ptr<DB::ISettingsAccessor> settingsAccessor;
	std::shared_ptr<INetwork> network;
	Json json;
	bool running;
	bool inited;
	Glib::Thread *thread;

};
}
#endif /* SYNCMANAGER_H_ */
