#include <gtest/gtest.h>
#include <libtimeit/Timer.h>
#include "libtimeit/logic/AutoTracker.h"


#include "../Mock/MockTimeKeeper.h"
#include "../Mock/MockDatabase.h"

namespace Test
{

using namespace libtimeit;

class AutoTrackerTest
{
public:
	AutoTrackerTest() :
			timeKeeper(
					std::shared_ptr<ITimeKeeper>(new MockTimeKeeper())),
					database(std::shared_ptr<IDatabase>(new test::MockDatabase()))
	{
		tracker = new AutoTracker(timeKeeper, database, timer);
	}

	~AutoTrackerTest()
	{
		delete tracker;
	}

private:
	AutoTracker* tracker;
	Timer timer;
	std::shared_ptr<ITimeKeeper> timeKeeper;
	std::shared_ptr<IDatabase> database;

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

