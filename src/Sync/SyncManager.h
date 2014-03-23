#ifndef SYNCMANAGER_H_
#define SYNCMANAGER_H_

#include <memory>
#include "Database.h"
#include "Timer.h"
#include "Network.h"
#include "Json.h"

class SyncManager: TimerObserver
{
public:
	SyncManager(std::shared_ptr<DB::IDatabase>&, std::shared_ptr<Timer>&, std::shared_ptr<INetwork>& network);
	virtual ~SyncManager();
	virtual void on_signal_10_seconds();
	void completeSync();
private:
	SyncManager();
	int syncTasks();
	int syncTimes();
	int syncTimesToDatabase(std::string result);
	int syncTaskToDatabase(std::string result);
	std::shared_ptr<DB::IDatabase> db;
	std::shared_ptr<DB::ITaskAccessor> taskAccessor;
	std::shared_ptr<DB::ITimeAccessor> timeAccessor;
	std::shared_ptr<DB::ISettingsAccessor> settingsAccessor;
	std::shared_ptr<Timer> timer;
	std::shared_ptr<INetwork> network;
	Json json;

};

#endif /* SYNCMANAGER_H_ */
