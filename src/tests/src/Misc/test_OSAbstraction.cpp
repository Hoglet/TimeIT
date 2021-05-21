#include <gtest/gtest.h>
#include <libtimeit/os_abstraction.h>
#include <iostream>
#include <fstream>

namespace Test
{
using namespace std;
using namespace libtimeit;

TEST( OSAbstraction, file_operations)
{
	string directory = "/tmp/osatest";
	string file = directory + "/test";
	make_directory(directory);
	ASSERT_EQ( false, file_exists(file)) << "File should not exist before created";
	ofstream out(file);
	out << "test";
	out.close();
	ASSERT_EQ( true, file_exists(file)) << "File should exist";
	unlink(file);
	ASSERT_EQ( false, file_exists(file)) << "File should not exist after deletion";
}


} /* namespace Test */
