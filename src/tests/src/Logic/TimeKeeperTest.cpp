#include "libtimeit/logic/TimeKeeper.h"
#include "libtimeit/Timer.h"
#include "../DB/TempDB.h"
#include <gtest/gtest.h>

using namespace std;
using namespace libtimeit;
using namespace test;

namespace Test
{

class MockIdleDetector: public IIdleDetector
{
public:
	virtual ~MockIdleDetector()
	{
	}
	;
	virtual int minutesIdle()
	{
		return m_minutesIdle;
	}
	virtual time_t timeIdle()
	{
		return m_timeIdle;
	}

	int m_minutesIdle;
	time_t m_timeIdle;
};

class TKObserver: public TimekeeperObserver
{
public:
	virtual void on_idleDetected()
	{
		idleDetected = true;
	}
	virtual void on_activityResumed()
	{
	}
	virtual void on_runningChanged()
	{
		runningChanged = true;
	}
	bool idleDetected;
	bool runningChanged;
};

TEST( TimeKeeper, starting_stoping_and_toggling)
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Timekeeper timeKeeper(db, timer);
	Task_accessor taskaccessor(db);
	Time_accessor timeaccessor(db);

	int64_t taskID = taskaccessor.create(Task("Test", 0));
	TKObserver observer;
	timeKeeper.attach(&observer);
	observer.runningChanged = false;
	vector<int64_t> runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks before starting ";
	timeKeeper.StartTask(taskID);
	runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.StopTask(taskID);
	runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after stopping ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.ToggleTask(taskID);
	runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after toggling ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	timeKeeper.ToggleTask(taskID);
	runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after toggling ";

	timeKeeper.detach(&observer);
}

TEST( TimeKeeper, update )
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Timekeeper   timeKeeper(db, timer);
	Task_accessor taskaccessor(db);
	Time_accessor timeaccessor(db);

	time_t now = time(0);
	int64_t taskID = taskaccessor.create(Task("Test", 0));
	TKObserver observer;
	timeKeeper.attach(&observer);

	timeKeeper.StartTask(taskID);

	vector<Time_entry> entries = timeaccessor.time_list(taskID, 0, now + 1000);
	int64_t teID = entries.at(0).ID();
	auto te = timeaccessor.by_ID(teID);
	timeaccessor.update(te->with_start(0).with_start(10));

	timeKeeper.on_signal_10_seconds();

	auto changedItem = timeaccessor.by_ID(teID);
	ASSERT_TRUE( 100 < changedItem->stop()) << "Stop should be higher than 100 ";

	timeKeeper.detach(&observer);
}

TEST( TimeKeeper, Stop_all)
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Timekeeper   timeKeeper(db, timer);
	Task_accessor taskaccessor( db);
	Time_accessor timeaccessor( db);

	int64_t taskID = taskaccessor.create(Task("Test", 0));
	TKObserver observer;
	timeKeeper.attach(&observer);

	timeKeeper.StartTask(taskID);
	observer.runningChanged = false;
	timeKeeper.stopAll();
	vector<int64_t> runningTasks = timeaccessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_TRUE( observer.runningChanged) << "Notification supplied ";
	timeKeeper.detach(&observer);
}


} /* namespace Test */
