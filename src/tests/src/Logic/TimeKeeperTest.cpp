#include "libtimeit/logic/time_keeper.h"
#include "libtimeit/timer.h"
#include "../DB/TempDB.h"
#include <gtest/gtest.h>

using namespace std;
using namespace libtimeit;
using namespace test;

namespace Test
{

class TKObserver: public time_keeper_observer
{
public:
	TKObserver(Time_keeper& tk): time_keeper_observer(tk) {};
	virtual void on_idle_detected(const Time_entry& /*te*/)
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
	notification_manager notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper timeKeeper(db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	int64_t taskID = tasks.create(task("Test", 0));
	TKObserver observer(timeKeeper);
	observer.runningChanged = false;
	vector<int64_t> runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks before starting ";
	timeKeeper.start(taskID);
	runningTasks = times.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.stop(taskID);
	runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after stopping ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.toggle(taskID);
	runningTasks = times.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after toggling ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	timeKeeper.toggle(taskID);
	runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after toggling ";

}

TEST( TimeKeeper, update )
{
	notification_manager notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper    timeKeeper(db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	time_t now = time(nullptr);
	int64_t taskID = tasks.create(task("Test", 0));
	TKObserver observer(timeKeeper);

	timeKeeper.start(taskID);

	vector<Time_entry> entries = times.time_list(taskID, 0, now + 1000);
	auto teID = entries.at(0).uuid;
	auto te = times.by_id(teID);
	times.update(te->with_start(0).with_start(10));

	timeKeeper.on_signal_10_seconds();

	auto changedItem = times.by_id(teID);
	ASSERT_TRUE( 100 < changedItem->stop) << "Stop should be higher than 100 ";

}

TEST( TimeKeeper, Stop_all)
{
	notification_manager notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper   time_keeper(db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	int64_t taskID = tasks.create(task("Test", 0));
	TKObserver observer(time_keeper);

	time_keeper.start(taskID);
	observer.runningChanged = false;
	time_keeper.stop_all();
	vector<int64_t> runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_TRUE( observer.runningChanged) << "Notification supplied ";
}


} /* namespace Test */
