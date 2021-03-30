#include <iostream>
#include <sstream>
#include <libtimeit/exceptions/db_exception.h>

namespace libtimeit
{

using namespace std;

db_exception::db_exception(string message, int return_code)
{
	this->message     = message;
	this->return_code = return_code;
}

db_exception::~db_exception() throw()
{
}


const char *db_exception::what() const throw()
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