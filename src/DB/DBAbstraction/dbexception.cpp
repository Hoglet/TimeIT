#include <iostream>
#include <sstream>
#include "dbexception.h"

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
	static stringstream str;
	str << rc2string() <<" "<< message.c_str() <<endl;
	return str.str().c_str();
}



void dbexception::setMessage(std::string op1)
{
	message=op1;	
}

void dbexception::setReturnCode(int op1)
{
	returnCode=op1;
}

const char* dbexception::rc2string() const
{
	static char tmp[10];
	static const char* retVal="";
	switch(returnCode)
	{
	default:
		sprintf(tmp,"Error %d",returnCode);
		retVal=tmp;
	}
	return retVal;
}
