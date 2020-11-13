#ifndef SYNCMANAGER_H_
#define SYNCMANAGER_H_

#include <memory>
#include <DB/Database.h>
#include <Timer.h>
#include <Network.h>
#include <Sync/Json.h>

namespace syncing
{
class SyncManager
{
public:
	SyncManager(std::shared_ptr<DB::IDatabase>&, std::shared_ptr<INetwork> &network, std::shared_ptr<Utils::MessageCenter> messageCenter);
	virtual ~SyncManager();
	void start();
	void stop();
	bool doSync(time_t pointInTime);
	private:
	SyncManager();
	bool isActive();
	void worker();
	bool syncTasks(time_t sincePointInTime);
	bool syncTimes(time_t sincePointInTime);
	int syncTimesToDatabase(std::string result);
	int syncTaskToDatabase(std::string result);
	void manageNetworkProblems(NetworkResponse result);

	std::shared_ptr<DB::IDatabase> db;
	std::shared_ptr<DB::ITaskAccessor> taskAccessor;
	std::shared_ptr<DB::ITimeAccessor> timeAccessor;
	std::shared_ptr<DB::ISettingsAccessor> settingsAccessor;
	std::shared_ptr<Utils::MessageCenter> messageCenter;
	std::shared_ptr<INetwork> network;
	bool running;
	bool inited;
	Glib::Thread *thread;
	int partialSync = 0;
	const int ONE_DAY = 60 * 60 * 24;

};
}
#endif /* SYNCMANAGER_H_ */
