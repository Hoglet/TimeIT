#include "libtimeit/logic/time_keeper.h"
#include "../DB/temp_db.h"
#include <gtest/gtest.h>

using namespace std;
using namespace libtimeit;
using namespace test;

namespace test
{

class tk_observer: public time_manager_observer
{
public:
	tk_observer( time_manager& tk): time_manager_observer( tk) {};
	virtual void on_idle_detected(const time_entry& /*te*/)
	{
		idle_detected = true;
	}
	virtual void on_activity_resumed()
	{
	}
	virtual void on_running_changed()
	{
		running_changed = true;
	}
	bool idle_detected = false;
	bool running_changed = false;
};

TEST( TimeKeeper, starting_stoping_and_toggling)
{
	/*
	notification_manager notifier;
	temp_db db(notifier);
	timer_base timer;
	time_manager time_keeper( db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	task test_task(task("Test", {}));
	tasks.create(test_task);
	tk_observer observer( time_keeper);
	observer.running_changed = false;
	auto running_tasks = times.currently_running();
	ASSERT_EQ( 0, running_tasks.size()) << "Checking number of running tasks before starting ";
	time_keeper.start( test_task.id);
	running_tasks = times.currently_running();
	ASSERT_EQ( 1, running_tasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_EQ( true, observer.running_changed) << "Notification supplied ";

	observer.running_changed = false;
	time_keeper.stop( test_task.id);
	running_tasks = times.currently_running();
	ASSERT_EQ( 0, running_tasks.size()) << "Checking number of running tasks after stopping ";
	ASSERT_EQ( true, observer.running_changed) << "Notification supplied ";

	observer.running_changed = false;
	time_keeper.toggle( test_task.id);
	running_tasks = times.currently_running();
	ASSERT_EQ( 1, running_tasks.size()) << "Checking number of running tasks after toggling ";
	ASSERT_EQ( true, observer.running_changed) << "Notification supplied ";

	time_keeper.toggle( test_task.id);
	running_tasks = times.currently_running();
	ASSERT_EQ( 0, running_tasks.size()) << "Checking number of running tasks after toggling ";
*/
}


TEST( TimeKeeper, update )
{
	notification_manager notifier;
	temp_db db(notifier);
	timer_base timer;
	time_manager    time_keeper( db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	auto now = system_clock::now();
	task test_task("Test", {});
	tasks.create(test_task);
	tk_observer observer( time_keeper);

	time_keeper.start( test_task.id);

	vector<time_entry> entries = times.by_activity(
			test_task.id,
			system_clock::from_time_t( 0 ),
			now + 1000s );
	auto te_id = entries.at( 0).id;
	auto te = times.by_id( te_id);
	times.update(te->with_start( system_clock::from_time_t( 0 )).with_stop( system_clock::from_time_t( 100 )));

	time_keeper.on_signal_10_seconds();

	auto changed_item = times.by_id( te_id);
	ASSERT_EQ(  100  , system_clock::to_time_t( changed_item->stop)) << "Stop should be 100 ";

}

TEST( TimeKeeper, Stop_all)
{
	notification_manager notifier;
	temp_db db(notifier);
	timer_base timer;
	time_manager   time_keeper( db, timer, notifier);
	task_accessor tasks(db);
	time_accessor times(db);

	task test_task( "Test", {} );
	tasks.create( test_task );
	tk_observer observer( time_keeper);

	time_keeper.start(test_task.id);
	observer.running_changed = false;
	time_keeper.stop_all();
	auto running_tasks = times.currently_running();
	ASSERT_EQ( 0, running_tasks.size()) << "Checking number of running tasks after starting ";
	ASSERT_TRUE( observer.running_changed) << "Notification supplied ";
}


} /* namespace test */
