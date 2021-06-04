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
		Database   &database,
		Timer       &timer)
		:
		Timer_observer(timer),
		time_keeper(time_keeper_),
		auto_track_accessor(database),
		task_accessor( database )
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
	if (old_workspace != new_workspace)
	{
		do_task_switching(old_workspace, new_workspace);
		old_workspace = new_workspace;
	}

	auto user_is_active = time_keeper.user_is_active();
	if( user_is_active && old_user_is_active != user_is_active )
	{
		for(auto task_id : auto_track_accessor.task_ids(new_workspace))
		{
			auto task = task_accessor.by_id(task_id);
			if(task.has_value() && task->quiet)
			{
				time_keeper.start(task_id);
			}
		}
	}
	old_user_is_active = user_is_active;
}

bool contains(vector<Task_id> vec, Task_id item)
{
	return (find(vec.begin(), vec.end(), item) != vec.end());
}

void Auto_tracker::do_task_switching(unsigned old_workspace, unsigned new_workspace)
{
	vector<Task_id> tasks_to_stop  = auto_track_accessor.task_ids(old_workspace);
	vector<Task_id> tasks_to_start = auto_track_accessor.task_ids(new_workspace);
	for (auto task_id : tasks_to_stop)
	{
		if (!contains(tasks_to_start, task_id))
		{
			time_keeper.stop(task_id);
		}
	}
	for (auto task_id : tasks_to_start)
	{
		try
		{
			time_keeper.start(task_id);
		}
		catch (...)
		{

		}
	}

}

}
//http://library.gnome.org/devel/libwnck/stable/WnckScreen.html#wnck-screen-calc-workspace-layout
