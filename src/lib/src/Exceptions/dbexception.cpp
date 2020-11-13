#include <iostream>
#include <sstream>
#include "Exceptions/dbexception.h"

using namespace std;
dbexception::dbexception()
{
	returnCode=0;
}

dbexception::~dbexception() throw()
{
}


const char* dbexception::what() const throw()
{
	stringstream str;
	str << rc2string() <<" "<< message.c_str() <<endl;
	static std::string result=str.str();
	return result.c_str();
}



void dbexception::setMessage(std::string op1)
{
	message=op1;
}

void dbexception::setReturnCode(int op1)
{
	returnCode=op1;
}

std::string dbexception::rc2string() const
{
	stringstream str;
	switch(returnCode)
	{
	default:
		str<<"Error "<<returnCode;
		break;
	}
	return str.str();
}
