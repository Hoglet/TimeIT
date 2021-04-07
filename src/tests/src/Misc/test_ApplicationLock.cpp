#include <fstream>
#include <gtest/gtest.h>
#include <libtimeit/misc/ApplicationLock.h>
#include <libtimeit/OSAbstraction.h>

namespace Test
{
using namespace libtimeit;

TEST( ApplicationLock, test)
{
	string filename="/tmp/locktest";
	ofstream out(filename);
	out<<"Lock me!";
	out.close();
	ApplicationLock lock(filename);
	ASSERT_EQ( true, lock.lockAquired()) << "Check that first lock succeded";

	ApplicationLock lock2("Not/existing/file");
	ASSERT_EQ( false, lock2.lockAquired()) << "Check that second lock failed";

	OSAbstraction::unlink(filename);
}

} /* namespace Test */
