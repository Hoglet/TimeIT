#include "test_OSAbstraction.h"
#include "cute.h"
#include "OSAbstraction.h"
#include <iostream>
#include <fstream>
namespace Test
{

void OSAbstraction_test_file_operations()
{
	std::string directory = "/tmp/osatest";
	std::string file = directory + "/test";
	OSAbstraction::mkDir(directory);
	ASSERT_EQUALM("File should not exist before created", false, OSAbstraction::fileExists(file));
	std::ofstream out(file);
	out << "test";
	out.close();
	ASSERT_EQUALM("File should exist", true, OSAbstraction::fileExists(file));
	OSAbstraction::unlink(file);
	ASSERT_EQUALM("File should not exist after deletion", false, OSAbstraction::fileExists(file));
}

cute::suite make_suite_OSAbstractionTest()
{
	cute::suite s;
	s.push_back(CUTE(OSAbstraction_test_file_operations));
	return s;
}

} /* namespace Test */
