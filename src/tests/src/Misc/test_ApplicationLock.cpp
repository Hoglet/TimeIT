#include <fstream>
#include <gtest/gtest.h>
#include <libtimeit/misc/ApplicationLock.h>
#include <libtimeit/OSAbstraction.h>

namespace Test
{

TEST( ApplicationLock, test)
{
	std::string filename="/tmp/locktest";
	std::ofstream out(filename);
	out<<"Lock me!";
	out.close();
	ApplicationLock lock(filename);
	ASSERT_EQ( true, lock.lockAquired()) << "Check that first lock succeded";

	ApplicationLock lock2("Not/existing/file");
	ASSERT_EQ( false, lock2.lockAquired()) << "Check that second lock failed";

	OSAbstraction::unlink(filename);
}

} /* namespace Test */
