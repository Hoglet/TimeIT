#include <iostream>
#include <sstream>
#include <libtimeit/exceptions/db_exception.h>

namespace libtimeit
{

using namespace std;

db_exception::db_exception( string_view op_message, int op_return_code):
	message( op_message ),
	return_code( op_return_code)
{
}

const char *db_exception::what() const noexcept
{
	stringstream str;
	str << rc_to_string() << " " << message.c_str() << endl;
	static std::string result = str.str();
	return result.c_str();
}

std::string db_exception::rc_to_string() const
{
	stringstream str;
	str << "Error ";
	if (return_code > 0)
	{
			str << return_code;
	}
	return str.str();
}

}
