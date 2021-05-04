#include "libtimeit/logic/time_keeper.h"
#include "libtimeit/timer.h"
#include "../DB/TempDB.h"
#include <gtest/gtest.h>

using namespace std;
using namespace libtimeit;
using namespace test;

namespace Test
{

class TKObserver: public Time_keeper_observer
{
public:
	virtual void on_idle_detected(Time_id id)
	{
		idleDetected = true;
	}
	virtual void on_activity_resumed()
	{
	}
	virtual void on_running_changed()
	{
		runningChanged = true;
	}
	bool idleDetected = false;
	bool runningChanged = false;
};

TEST( TimeKeeper, starting_stoping_and_toggling)
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper timeKeeper(db, timer, notifier);
	Task_accessor task_accessor(db);
	Time_accessor time_accessor(db);

	int64_t taskID = task_accessor.create(Task("Test", 0));
	TKObserver observer;
	timeKeeper.attach(&observer);
	observer.runningChanged = false;
	vector<int64_t> runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks before starting ";
	timeKeeper.start(taskID);
	runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.stop(taskID);
	runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after stopping ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.toggle(taskID);
	runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after toggling ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	timeKeeper.toggle(taskID);
	runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after toggling ";

	timeKeeper.detach(&observer);
}

TEST( TimeKeeper, update )
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper    timeKeeper(db, timer, notifier);
	Task_accessor task_accessor(db);
	Time_accessor time_accessor(db);

	time_t now = time(nullptr);
	int64_t taskID = task_accessor.create(Task("Test", 0));
	TKObserver observer;
	timeKeeper.attach(&observer);

	timeKeeper.start(taskID);

	vector<Time_entry> entries = time_accessor.time_list(taskID, 0, now + 1000);
	int64_t teID = entries.at(0).id;
	auto te = time_accessor.by_ID(teID);
	time_accessor.update(te->with_start(0).with_start(10));

	timeKeeper.on_signal_10_seconds();

	auto changedItem = time_accessor.by_ID(teID);
	ASSERT_TRUE( 100 < changedItem->stop) << "Stop should be higher than 100 ";

	timeKeeper.detach(&observer);
}

TEST( TimeKeeper, Stop_all)
{
	Notifier notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper   time_keeper(db, timer, notifier);
	Task_accessor task_accessor(db);
	Time_accessor time_accessor(db);

	int64_t taskID = task_accessor.create(Task("Test", 0));
	TKObserver observer;
	time_keeper.attach(&observer);

	time_keeper.start(taskID);
	observer.runningChanged = false;
	time_keeper.stop_all();
	vector<int64_t> runningTasks = time_accessor.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_TRUE( observer.runningChanged) << "Notification supplied ";
	time_keeper.detach(&observer);
}


} /* namespace Test */
