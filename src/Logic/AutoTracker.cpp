#include <AutoTracker.h>
#include <Timer.h>
#include <iostream>
#include <vector>
#include <Timekeeper.h>
#include <Timer.h>

AutoTracker::AutoTracker(boost::shared_ptr<ITimeKeeper>& timekeeper, const boost::shared_ptr<DB::Database>& database,
		const boost::shared_ptr<Timer>& timer)
{
	m_timekeeper = timekeeper;
	m_timer = timer;
	m_autotrackAccessor = database->getAutotrackAccessor();
	m_taskAccessor = database->getTaskAccessor();
	oldWorkspace = -1;
	on_signal_10_seconds();//check if we should start anything;
	m_timer->attach(this);
	m_taskAccessor->attach(this);
	//m_taskAccessor->attach(this);
}

AutoTracker::~AutoTracker()
{
	m_timer->detach(this);
	m_taskAccessor->detach(this);
	//m_taskAccessor->detach(this);
}

void AutoTracker::on_signal_10_seconds()
{

}
void AutoTracker::on_signal_1_second()
{
	check4Changes();
}

void AutoTracker::check4Changes()
{
	//Check for changes
	int newWorkspace;
	newWorkspace = m_workspace.get_active();
	if (newWorkspace >= 0 && oldWorkspace != newWorkspace)
	{
		stopTasks(oldWorkspace);
		startTasks(newWorkspace);
		oldWorkspace = newWorkspace;
	}
}

void AutoTracker::stopTasks(int workspace)
{
	std::vector<int64_t> tasksToStop = m_autotrackAccessor->getTaskIDs(workspace);
	std::vector<int64_t>::iterator iter = tasksToStop.begin();
	for (; iter != tasksToStop.end(); iter++)
	{
		int64_t taskID = *iter;
		Task task = m_taskAccessor->getTask(taskID);
		if (task.getAutotrack())
		{
			m_timekeeper->StopTask(taskID);
		}
	}
}
void AutoTracker::startTasks(int workspace)
{
	std::vector<int64_t> tasksToStart = m_autotrackAccessor->getTaskIDs(workspace);
	std::vector<int64_t>::iterator iter;
	for (iter = tasksToStart.begin(); iter < tasksToStart.end(); iter++)
	{
		int64_t taskID = *iter;
		try
		{
			Task task = m_taskAccessor->getTask(taskID);
			if (task.getAutotrack())
			{
				m_timekeeper->StartTask(taskID);
			}
		} catch (...)
		{

		}
	}
}
void AutoTracker::on_taskAutotrackChanged(int64_t taskID, bool enable)
{
// Ignoring this event. Need a better strategy
	//TODO Find a better strategy

/*	std::vector<int> workspaces = m_autotrackAccessor->getWorkspaces(taskID);
	Task task = m_taskAccessor->getTask(taskID);
	std::vector<int>::iterator iter;
	for (iter = workspaces.begin(); iter != workspaces.end(); iter++)
	{
		int workspace = *iter;
		if (workspace == oldWorkspace)
		{
			if (task.getAutotrack())
			{
				m_timekeeper->StartTask(taskID);
			}
			else
			{
				m_timekeeper->StopTask(taskID);
			}
		}
	}*/
}

//http://library.gnome.org/devel/libwnck/stable/WnckScreen.html#wnck-screen-calc-workspace-layout
