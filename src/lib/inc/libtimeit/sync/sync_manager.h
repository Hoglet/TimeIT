#ifndef SYNC_MANAGER_H_
#define SYNC_MANAGER_H_

#include <memory>
#include <libtimeit/db/database.h>
#include <libtimeit/timer.h>
#include <libtimeit/sync/network.h>
#include <libtimeit/sync/json_converter.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/task_accessor.h>

namespace libtimeit
{
using namespace std;

enum class Sync_state
{
	IDLE,
	TASK_REQUEST,
	WAIT,
	TASK_STORE,
	TIME_REQUEST,
	TIME_STORE,
	FAIL
};

class Sync_manager : public Timer_observer
{
public:
	Sync_manager(
			Database& database,
			INetwork& network,
			Notifier& notifier,
			Timer&    timer);

	Sync_state status();
	void on_signal_1_second() override;

	void reset();
private:
	bool    is_active();
	time_t  get_next_sync(time_t reference_point);
	bool    request_is_done();
	void    sync_times_to_database();
	void    sync_tasks_to_database();
	void    manage_network_problems();

	shared_ptr<asyncHTTPResponse> request_tasks(time_t sincePointInTime);
	shared_ptr<asyncHTTPResponse> request_times(time_t sincePointInTime);

	Task_accessor     task_accessor;
	Time_accessor     time_accessor;
	Settings_accessor settings_accessor;
	INetwork&         network;

	Sync_state                     state          {Sync_state::IDLE};
	Sync_state                     following_state{Sync_state::IDLE};
	shared_ptr <asyncHTTPResponse> outstandingRequest;

	Notifier&  notifier_;
	time_t     next_sync{0};
	time_t     next_full_sync{0};
	time_t     last_sync{0};
	time_t     current_sync{0};
};
}
#endif /* SYNC_MANAGER_H_ */
