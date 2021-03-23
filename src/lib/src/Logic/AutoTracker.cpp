#include <iostream>
#include <vector>
#include <algorithm>
#include <libtimeit/Timer.h>
#include <libtimeit/logic/AutoTracker.h>
#include <libtimeit/logic/TimeKeeper.h>

namespace libtimeit
{

using namespace libtimeit;
using namespace std;

AutoTracker::AutoTracker(
		ITimeKeeper &op_time_keeper,
		Database   &op_database,
		Timer       &op_timer)
		:
		timer(op_timer),
		time_keeper(op_time_keeper),
		autotrack_accessor(op_database),
		task_accessor(op_database)
{
	oldWorkspace = -1;
	on_signal_1_second(); //check if we should start anything;
	timer.attach(this);
}

AutoTracker::~AutoTracker()
{
	timer.detach(this);
}

void AutoTracker::on_signal_1_second()
{
	check4Changes();
}

void AutoTracker::check4Changes()
{
	int newWorkspace;
	newWorkspace = workspace.get_active();
	if (newWorkspace >= 0 && oldWorkspace != newWorkspace)
	{
		doTaskSwitching(oldWorkspace, newWorkspace);
		oldWorkspace = newWorkspace;
	}
}

bool contains(std::vector<int64_t> vec, int64_t item)
{
	if (find(vec.begin(), vec.end(), item) == vec.end())
	{
		return true;
	}
	else
	{
		return false;
	}

}

void AutoTracker::doTaskSwitching(int oldWorkspace, int newWorkspace)
{
	std::vector<int64_t> tasksToStop = autotrack_accessor.getTaskIDs(oldWorkspace);
	std::vector<int64_t> tasksToStart = autotrack_accessor.getTaskIDs(newWorkspace);
	for (int64_t taskID : tasksToStop)
	{
		if (false == contains(tasksToStart, taskID))
		{
			time_keeper.StopTask(taskID);
		}
	}
	for (int64_t taskID : tasksToStart)
	{
		try
		{
			time_keeper.StartTask(taskID);
		}
		catch (...)
		{

		}
	}

}

}
//http://library.gnome.org/devel/libwnck/stable/WnckScreen.html#wnck-screen-calc-workspace-layout
