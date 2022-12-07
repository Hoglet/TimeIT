#ifndef AUTO_TRACKER_H
#define AUTO_TRACKER_H
#include <iostream>
#include <libtimeit/timer.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/logic/workspace.h>
#include <climits>

namespace libtimeit
{
using namespace std;

class auto_tracker : public timer_observer
{
public:
	auto_tracker(
			time_manager& /*time_keeper*/,
			database&    /*db*/,
			timer_base&       /*timer*/
			);

private:
	void check_for_changes();
	void do_task_switching(unsigned new_workspace);
	//TimerObserver
	void on_signal_1_second() override;

	unsigned old_workspace {UINT_MAX};

	time_manager        &time_keeper;

	auto_track_accessor  auto_track;
	task_accessor        tasks;
	x_11_workspace            workspace;
	bool                 old_user_is_active{false};
};

}
#endif /*AUTO_TRACKER_H*/
