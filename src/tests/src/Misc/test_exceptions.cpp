#include <string>
#include <gtest/gtest.h>
#include <libtimeit/exceptions/db_exception.h>
#include <libtimeit/exceptions/general_exception.h>

namespace Test
{

using namespace libtimeit;
using namespace std;

TEST( Exception, dbException )
{
	db_exception dbe("I'm a teapot", 418);
	string expected("Error 418 I'm a teapot\n");
	ASSERT_EQ(expected, dbe.what());
}

TEST( Exceptions, generalException)
{
	General_exception ge("Something happened");
	string expected("Something happened");
	ASSERT_EQ(expected, ge.what());
}

} /* namespace Test */
