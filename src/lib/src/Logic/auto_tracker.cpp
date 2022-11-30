#include <libtimeit/timer.h>
#include <libtimeit/logic/auto_tracker.h>
#include <libtimeit/misc/contains.h>

namespace libtimeit
{

using namespace libtimeit;
using namespace std;

auto_tracker::auto_tracker(
		time_manager &op_time_keeper,
		database   &db,
		Timer       &timer)
		:
		timer_observer(timer),
		time_keeper( op_time_keeper),
		auto_track(db),
		tasks(db )
{
	check_for_changes(); //check if we should start anything;
}


void auto_tracker::on_signal_1_second()
{
	check_for_changes();
}

void auto_tracker::check_for_changes()
{
	unsigned new_workspace = workspace.active();
	if (old_workspace != new_workspace)
	{
		do_task_switching(new_workspace);
		old_workspace = new_workspace;
	}

	auto user_is_active = time_keeper.user_is_active();
	if( user_is_active && old_user_is_active != user_is_active )
	{
		for(auto id : auto_track.task_ids(new_workspace))
		{
			auto item = tasks.by_id(id);
			if(item.has_value() && item->quiet)
			{
				time_keeper.start(id);
			}
		}
	}
	old_user_is_active = user_is_active;
}


void auto_tracker::do_task_switching(unsigned new_workspace)
{
	auto tasks_to_stop  = auto_track.task_ids(old_workspace);
	auto tasks_to_start = auto_track.task_ids(new_workspace);
	for (auto id : tasks_to_stop)
	{
		if (!contains(tasks_to_start, id))
		{
			time_keeper.stop(id);
		}
	}
	for (auto id : tasks_to_start)
	{
		try
		{
			time_keeper.start(id);
		}
		catch (...)
		{

		}
	}

}

}
//http://library.gnome.org/devel/libwnck/stable/WnckScreen.html#wnck-screen-calc-workspace-layout
