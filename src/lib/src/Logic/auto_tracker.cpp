#include <vector>
#include <algorithm>
#include <libtimeit/timer.h>
#include <libtimeit/logic/auto_tracker.h>
#include <libtimeit/logic/time_keeper.h>

namespace libtimeit
{

using namespace libtimeit;
using namespace std;

Auto_tracker::Auto_tracker(
		Time_keeper &time_keeper_,
		Database   &database_,
		Timer       &timer_)
		:
		time_keeper(time_keeper_),
		auto_track_accessor(database_),
		Timer_observer(timer_)
{
	check_for_changes(); //check if we should start anything;
}


void Auto_tracker::on_signal_1_second()
{
	check_for_changes();
}

void Auto_tracker::check_for_changes()
{
	unsigned new_workspace = workspace.active();
	if (new_workspace >= 0 && old_workspace != new_workspace)
	{
		do_task_switching(old_workspace, new_workspace);
		old_workspace = new_workspace;
	}
}

bool contains(vector<Task_id> vec, Task_id item)
{
	return (find(vec.begin(), vec.end(), item) != vec.end());
}

void Auto_tracker::do_task_switching(int old_workspace, int new_workspace)
{
	vector<Task_id> tasks_to_stop  = auto_track_accessor.task_IDs(old_workspace);
	vector<Task_id> tasks_to_start = auto_track_accessor.task_IDs(new_workspace);
	for (int64_t task_ID : tasks_to_stop)
	{
		if (!contains(tasks_to_start, task_ID))
		{
			time_keeper.stop(task_ID);
		}
	}
	for (auto task_ID : tasks_to_start)
	{
		try
		{
			time_keeper.start(task_ID);
		}
		catch (...)
		{

		}
	}

}

}
//http://library.gnome.org/devel/libwnck/stable/WnckScreen.html#wnck-screen-calc-workspace-layout
