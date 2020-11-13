#include "test_ApplicationLock.h"
#include "ApplicationLock.h"
#include "cute.h"
#include "libtimeit/OSAbstraction.h"
#include <fstream>
namespace Test
{

void ApplicationLock_test()
{
	std::string filename="/tmp/locktest";
	std::ofstream out(filename);
	out<<"Lock me!";
	out.close();
	ApplicationLock lock(filename);
	ASSERT_EQUALM("Check that first lock succeded", true, lock.lockAquired());

	ApplicationLock lock2("Not/existing/file");
	ASSERT_EQUALM("Check that second lock failed", false, lock2.lockAquired());

	OSAbstraction::unlink(filename);
}

cute::suite make_suite_ApplicationLockTest()
{
	cute::suite s;
	s.push_back(CUTE(ApplicationLock_test));
	return s;
}
} /* namespace Test */
