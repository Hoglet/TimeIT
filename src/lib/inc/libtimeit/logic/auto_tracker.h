#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_
#include <iostream>
#include <libtimeit/timer.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <libtimeit/logic/workspace.h>

namespace libtimeit
{
using namespace std;

class Auto_tracker : public Timer_observer
{
public:
	Auto_tracker(
			Time_keeper& time_keeper_,
			Database&    database_,
			Timer&       timer_
			);

private:
	void check_for_changes();
	void do_task_switching(int old_workspace, int new_workspace);
	//TimerObserver
	void on_signal_1_second() override;

	int old_workspace;

	Time_keeper        &time_keeper;
	Timer              &timer;

	Auto_track_accessor  auto_track_accessor;
	Workspace           workspace;
};

}
#endif /*AUTOTRACKER_H_*/
