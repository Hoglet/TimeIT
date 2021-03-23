#include <gtest/gtest.h>
#include <libtimeit/Timer.h>
#include "libtimeit/logic/AutoTracker.h"


#include "libtimeit/logic/TimeKeeper.h"

namespace Test
{

using namespace libtimeit;

class AutoTrackerTest
{
public:
	AutoTrackerTest()
	{
		//tracker = new AutoTracker(timeKeeper, database, timer);
	}

	~AutoTrackerTest()
	{
		//delete tracker;
	}

private:
/*	AutoTracker* tracker;
	Timer timer;
	TimeKeeper timeKeeper;
	TempDB database;
*/
};

TEST( Auto_tracker, Workspace_switch_test)
{
	AutoTrackerTest test;
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

