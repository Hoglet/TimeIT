#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_
#include <iostream>
#include <Timer.h>
#include <ITimeKeeper.h>
#include <Database.h>
#include <IAutotrackAccessor.h>
#include <Workspace.h>


class AutoTracker:  public TimerObserver, public TaskAccessorObserver
{
public:
	AutoTracker(boost::shared_ptr<ITimeKeeper>& timekeeper, const boost::shared_ptr<DB::Database>& database, const boost::shared_ptr<Timer>& timer);
	virtual ~AutoTracker();
private:
	void check4Changes();

	//TimerObserver
	virtual void on_signal_1_second() ;
	virtual void on_signal_10_seconds();

	//TaskObserver
	virtual void on_taskAdded(const Task&)  {};
	virtual void on_taskUpdated(const Task&) {};
	virtual void on_taskRemoved(int64_t) {};

	//
	void stopTasks(int workspace);
	void startTasks(int workspace);
	int oldWorkspace;
	boost::shared_ptr<ITimeKeeper> m_timekeeper;
	boost::shared_ptr<IAutotrackAccessor> m_autotrackAccessor;
	boost::shared_ptr<ITaskAccessor> m_taskAccessor;
	boost::shared_ptr<Timer> m_timer;
	Workspace m_workspace;
};

#endif /*AUTOTRACKER_H_*/
