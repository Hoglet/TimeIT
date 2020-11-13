#include "test_exceptions.h"
#include "cute.h"
#include "Exceptions/dbexception.h"
#include "Exceptions/GeneralException.h"
#include <string>

namespace Test
{

void Exception_test_dbException()
{
	dbexception dbe;
	dbe.setMessage("I'm a teapot");
	dbe.setReturnCode(418);
	std::string expected("Error 418 I'm a teapot\n");
	ASSERT_EQUAL(expected, dbe.what());
}

void Exceptions_test_generalException()
{
	GeneralException ge;
	ge.setMessage("Something happened");
	std::string expected("Something happened");
	ASSERT_EQUAL(expected, ge.what());
}

cute::suite make_suite_exceptionTest()
{
	cute::suite s;
	s.push_back(CUTE(Exception_test_dbException));
	s.push_back(CUTE(Exceptions_test_generalException));
	return s;
}

} /* namespace Test */
