#include "TestRunner.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "Logic/AutoTrackerTest.h"
#include "Logic/ControllerTest.h"
#include "Misc/test_exceptions.h"
#include "Misc/test_OSAbstraction.h"
#include "Misc/test_ApplicationLock.h"
#include "cute_xml_file.h"
#include "xml_listener.h"

#include <gtkmm.h>

namespace Test
{
TestRunner::TestRunner()
{
	s += make_suite_ControllerTest();
	s += make_suite_AutoTrackerTest();
	s += make_suite_AutoTrackerTest();
	s += make_suite_exceptionTest();
	s += make_suite_OSAbstractionTest();
	s += make_suite_ApplicationLockTest();
}

void TestRunner::run(int argc, char *argv[])
{
	using namespace std;
	Gtk::Main main(argc, argv);
	cute::xml_file_opener xmlfile(0, argv);
	cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);

	cute::makeRunner(lis, 0, argv)(s, "All");
}
}
