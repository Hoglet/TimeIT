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

class sync_manager : public timer_observer
{
public:
	sync_manager(
			database& db,
			abstract_network& network,
			notification_manager& notifier,
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

	shared_ptr<async_http_response> request_tasks(time_t sincePointInTime);
	shared_ptr<async_http_response> request_times(time_t sincePointInTime);

	task_accessor     tasks;
	time_accessor     times;
	settings_accessor settings;
	abstract_network&         network;

	Sync_state                     state          {Sync_state::IDLE};
	Sync_state                     following_state{Sync_state::IDLE};
	shared_ptr <async_http_response> outstanding_request;

	notification_manager&  notifier;
	time_t     next_sync{0};
	time_t     next_full_sync{0};
	time_t     last_sync{0};
	time_t     current_sync{0};
};
}
#endif /* SYNC_MANAGER_H_ */
