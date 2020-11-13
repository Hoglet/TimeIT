#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_
#include <iostream>
#include <Timer.h>
#include <TimeKeeper.h>
#include <DB/Database.h>
#include <DB/AutotrackAccessor.h>
#include <Workspace.h>

class AutoTracker: public TimerObserver
{
public:
	AutoTracker(std::shared_ptr<ITimeKeeper> &timekeeper, const std::shared_ptr<DB::IDatabase> &database, const std::shared_ptr<Timer> &timer);
	virtual ~AutoTracker();
	private:
	void check4Changes();
	void doTaskSwitching(int oldWorkspace, int newWorkspace);
	//TimerObserver
	virtual void on_signal_1_second();

	int oldWorkspace;
	std::shared_ptr<ITimeKeeper> m_timekeeper;
	std::shared_ptr<DB::IAutotrackAccessor> m_autotrackAccessor;
	std::shared_ptr<DB::ITaskAccessor> m_taskAccessor;
	std::shared_ptr<Timer> m_timer;
	Workspace m_workspace;
};

#endif /*AUTOTRACKER_H_*/
