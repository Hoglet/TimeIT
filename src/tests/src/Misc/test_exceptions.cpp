#include <string>
#include <gtest/gtest.h>
#include <libtimeit/exceptions/dbexception.h>
#include <libtimeit/exceptions/GeneralException.h>

namespace Test
{

using namespace std;

TEST( Exception, dbException )
{
	dbexception dbe;
	dbe.setMessage("I'm a teapot");
	dbe.setReturnCode(418);
	string expected("Error 418 I'm a teapot\n");
	ASSERT_EQ(expected, dbe.what());
}

TEST( Exceptions, generalException)
{
	GeneralException ge;
	ge.setMessage("Something happened");
	string expected("Something happened");
	ASSERT_EQ(expected, ge.what());
}

} /* namespace Test */
