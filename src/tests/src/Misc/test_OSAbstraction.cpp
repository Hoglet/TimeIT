#include <gtest/gtest.h>
#include <libtimeit/OSAbstraction.h>
#include <iostream>
#include <fstream>

namespace Test
{

TEST( OSAbstraction, file_operations)
{
	std::string directory = "/tmp/osatest";
	std::string file = directory + "/test";
	OSAbstraction::mkDir(directory);
	ASSERT_EQ( false, OSAbstraction::fileExists(file)) <<"File should not exist before created";
	std::ofstream out(file);
	out << "test";
	out.close();
	ASSERT_EQ( true, OSAbstraction::fileExists(file)) << "File should exist";
	OSAbstraction::unlink(file);
	ASSERT_EQ( false, OSAbstraction::fileExists(file)) << "File should not exist after deletion";
}


} /* namespace Test */
