#ifndef SYNCMANAGER_H_
#define SYNCMANAGER_H_

#include <memory>
#include <libtimeit/db/Database.h>
#include <libtimeit/Timer.h>
#include <libtimeit/sync/Network.h>
#include <libtimeit/sync/Json.h>

namespace libtimeit
{
using namespace std;

enum class SyncState
{
	IDLE,
	TASK_REQUEST,
	WAIT,
	TASK_STORE,
	TIME_REQUEST,
	TIME_STORE,
	FAIL
};

class SyncManager : public TimerObserver
{
public:
	SyncManager(
			IDatabase &database,
			INetwork  &network,
			Notifier  &notifier,
			Timer     &timer);
	virtual ~SyncManager();

	SyncState status();
	void on_signal_1_second();

	void reset();
private:
	SyncManager();

	bool    isActive();
	time_t  getNextSync(time_t referencePoint);
	bool    requestDone();
	void    syncTimesToDatabase();
	void    syncTasksToDatabase();
	void    manageNetworkProblems();

	shared_ptr<asyncHTTPResponse> requestTasks(time_t sincePointInTime);
	shared_ptr<asyncHTTPResponse> requestTimes(time_t sincePointInTime);

	shared_ptr<ITaskAccessor>     taskAccessor;
	shared_ptr<ITimeAccessor>     timeAccessor;
	shared_ptr<ISettingsAccessor> settingsAccessor;
	INetwork&     network;

	SyncState                           state          {SyncState::IDLE};
	SyncState                           following_state{SyncState::IDLE};
	shared_ptr <asyncHTTPResponse> outstandingRequest;


	Timer&     timer_;
	Notifier&  notifier_;
	time_t     nextSync{0};
	time_t     nextFullSync{0};
	time_t     lastSync{0};
	time_t     currentSync{0};
};
}
#endif /* SYNCMANAGER_H_ */
