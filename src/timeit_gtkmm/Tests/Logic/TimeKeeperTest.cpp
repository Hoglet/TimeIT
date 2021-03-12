#include "TimeKeeperTest.h"
#include "TimeKeeper.h"
#include "DB/TempDB.h"
#include "libtimeit/Timer.h"
#include "cute.h"

using namespace std;
using namespace libtimeit;

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

void TimeKeeper_testStartingStopingAndToggling()
{
	Notifier notifier;
	shared_ptr<IDatabase> db = shared_ptr<IDatabase>(new TempDB(notifier));
	Timer timer;
	shared_ptr<Timekeeper> timeKeeper = shared_ptr<Timekeeper>(new Timekeeper(db, timer));
	shared_ptr<ITaskAccessor> taskaccessor = db->getTaskAccessor();
	shared_ptr<ITimeAccessor> timeaccessor = db->getTimeAccessor();

	int64_t taskID = taskaccessor->newTask("Test", 0);
	TKObserver observer;
	timeKeeper->attach(&observer);
	observer.runningChanged = false;
	vector<int64_t> runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks before starting ", 0, runningTasks.size());
	timeKeeper->StartTask(taskID);
	runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks after starting ", 1, runningTasks.size());
	ASSERT_EQUALM("Notification supplied ", true, observer.runningChanged);

	observer.runningChanged = false;
	timeKeeper->StopTask(taskID);
	runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks after stopping ", 0, runningTasks.size());
	ASSERT_EQUALM("Notification supplied ", true, observer.runningChanged);

	observer.runningChanged = false;
	timeKeeper->ToggleTask(taskID);
	runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks after toggling ", 1, runningTasks.size());
	ASSERT_EQUALM("Notification supplied ", true, observer.runningChanged);

	timeKeeper->ToggleTask(taskID);
	runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks after toggling ", 0, runningTasks.size());

	timeKeeper->detach(&observer);
}

void TimeKeeper_testUpdate()
{
	Notifier notifier;
	shared_ptr<IDatabase> db = shared_ptr<IDatabase>(new TempDB(notifier));
	Timer timer;
	shared_ptr<Timekeeper> timeKeeper = shared_ptr<Timekeeper>(new Timekeeper(db, timer));
	shared_ptr<ITaskAccessor> taskaccessor = db->getTaskAccessor();
	shared_ptr<ITimeAccessor> timeaccessor = db->getTimeAccessor();

	time_t now = time(0);
	int64_t taskID = taskaccessor->newTask("Test", 0);
	TKObserver observer;
	timeKeeper->attach(&observer);

	timeKeeper->StartTask(taskID);

	vector<TimeEntry> entries = timeaccessor->getDetailTimeList(taskID, 0, now + 1000);
	int64_t teID = entries.at(0).ID();
	auto te = timeaccessor->getByID(teID);
	timeaccessor->update(te->withStart(0).withStop(10));

	timeKeeper->on_signal_10_seconds();

	auto changedItem = timeaccessor->getByID(teID);
	ASSERTM("Stop should be higher than 100 ", 100 < changedItem->stop());

	timeKeeper->detach(&observer);
}

void TimeKeeper_testStopAll()
{
	Notifier notifier;
	shared_ptr<IDatabase> db = shared_ptr<IDatabase>(new TempDB(notifier));
	Timer timer;
	shared_ptr<Timekeeper> timeKeeper = shared_ptr<Timekeeper>(new Timekeeper(db, timer));
	shared_ptr<ITaskAccessor> taskaccessor = db->getTaskAccessor();
	shared_ptr<ITimeAccessor> timeaccessor = db->getTimeAccessor();

	int64_t taskID = taskaccessor->newTask("Test", 0);
	TKObserver observer;
	timeKeeper->attach(&observer);

	timeKeeper->StartTask(taskID);
	observer.runningChanged = false;
	timeKeeper->stopAll();
	vector<int64_t> runningTasks = timeaccessor->getRunningTasks();
	ASSERT_EQUALM("Checking number of running tasks after starting ", 0, runningTasks.size());
	ASSERT_EQUALM("Notification supplied ", true, observer.runningChanged);
	timeKeeper->detach(&observer);
}

cute::suite make_suite_TimeKeeperTest()
{
	cute::suite s;
	s.push_back(CUTE(TimeKeeper_testStartingStopingAndToggling));
	s.push_back(CUTE(TimeKeeper_testUpdate));
	s.push_back(CUTE(TimeKeeper_testStopAll));
	return s;
}

} /* namespace Test */
