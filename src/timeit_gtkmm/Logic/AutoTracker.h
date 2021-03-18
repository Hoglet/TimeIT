#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_
#include <iostream>
#include <libtimeit/Timer.h>
#include <TimeKeeper.h>
#include <libtimeit/db/Database.h>
#include <libtimeit/db/AutotrackAccessor.h>
#include <libtimeit/logic/Workspace.h>


class AutoTracker: public libtimeit::TimerObserver
{
public:
	AutoTracker(std::shared_ptr<ITimeKeeper> &timeKeeper, const std::shared_ptr<libtimeit::IDatabase> &database, libtimeit::Timer& timer);
	virtual ~AutoTracker();
	private:
	void check4Changes();
	void doTaskSwitching(int oldWorkspace, int newWorkspace);
	//TimerObserver
	virtual void on_signal_1_second();

	int oldWorkspace;
	std::shared_ptr<ITimeKeeper> m_timeKeeper;
	std::shared_ptr<libtimeit::IAutotrackAccessor> m_autotrackAccessor;
	std::shared_ptr<libtimeit::ITaskAccessor> m_taskAccessor;
	libtimeit::Timer& m_timer;
	Workspace m_workspace;
};

#endif /*AUTOTRACKER_H_*/
