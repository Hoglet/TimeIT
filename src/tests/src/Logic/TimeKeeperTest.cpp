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
	virtual void on_idle_detected(const time_entry& /*te*/)
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

	task test_task(task("Test", {}));
	tasks.create(test_task);
	TKObserver observer(timeKeeper);
	observer.runningChanged = false;
	auto runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks before starting ";
	timeKeeper.start(test_task.id);
	runningTasks = times.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.stop(test_task.id);
	runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after stopping ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	observer.runningChanged = false;
	timeKeeper.toggle(test_task.id);
	runningTasks = times.currently_running();
	ASSERT_EQ( 1, runningTasks.size()) << "Checking number of running tasks after toggling ";
	ASSERT_EQ( true, observer.runningChanged) << "Notification supplied ";

	timeKeeper.toggle(test_task.id);
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

	auto now = system_clock::now();
	task test_task("Test", {});
	tasks.create(test_task);
	TKObserver observer(timeKeeper);

	timeKeeper.start(test_task.id);

	vector<time_entry> entries = times.by_activity(
			test_task.id,
			system_clock::from_time_t( 0 ),
			now + 1000s );
	auto teID = entries.at(0).id;
	auto te = times.by_id(teID);
	times.update(te->with_start( system_clock::from_time_t( 0 )).with_stop( system_clock::from_time_t( 100 )));

	timeKeeper.on_signal_10_seconds();

	auto changedItem = times.by_id(teID);
	ASSERT_EQ(  100  , system_clock::to_time_t( changedItem->stop)) << "Stop should be 100 ";

}

TEST( TimeKeeper, Stop_all)
{
	notification_manager notifier;
	TempDB db(notifier);
	Timer timer;
	Time_keeper   time_keeper(db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	task test_task( "Test", {} );
	tasks.create( test_task );
	TKObserver observer(time_keeper);

	time_keeper.start(test_task.id);
	observer.runningChanged = false;
	time_keeper.stop_all();
	auto runningTasks = times.currently_running();
	ASSERT_EQ( 0, runningTasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_TRUE( observer.runningChanged) << "Notification supplied ";
}


} /* namespace Test */
