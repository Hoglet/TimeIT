#include <fstream>
#include <gtest/gtest.h>
#include <libtimeit/misc/application_lock.h>
#include <libtimeit/OS_abstraction.h>

namespace Test
{
using namespace libtimeit;

TEST( ApplicationLock, test)
{
	string filename="/tmp/locktest";
	ofstream out(filename);
	out<<"Lock me!";
	out.close();
	Application_lock lock(filename);
	ASSERT_EQ( true, lock.lock_acquired()) << "Check that first lock succeded";

	Application_lock lock2("Not/existing/file");
	ASSERT_EQ( false, lock2.lock_acquired()) << "Check that second lock failed";

	unlink(filename);
}

} /* namespace Test */
