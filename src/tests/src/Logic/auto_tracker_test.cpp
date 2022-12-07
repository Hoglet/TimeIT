#include <gtest/gtest.h>
#include <libtimeit/timer.h>

namespace test
{

using namespace libtimeit;

class auto_tracker_test
{
public:
	auto_tracker_test()
	{
		//tracker = new AutoTracker(timeKeeper, database, timer);
	}

	~auto_tracker_test()
	{
		//delete tracker;
	}

private:
/*	AutoTracker* tracker;
	timer_base timer;
	TimeKeeper timeKeeper;
	TempDB database;
*/
};

TEST( Auto_tracker, Workspace_switch_test)
{
	auto_tracker_test test;
	//TODO: Write this test.....
	/*		database->get
	 set task 2 to start on workspace 2
	 set workspace 1
	 Check no starts
	 Set workspace 2
	 Check that task 2 was started
	 ASSERTM("Write test", false);
	 */
}


}
