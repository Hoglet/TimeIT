/*
 * GeneralException.cpp
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#include <libtimeit/exceptions/general_exception.h>

General_exception::General_exception()
{
	message = 0;
}

void General_exception::setMessage(const char* op1)
{
	message = op1;
}

const char* General_exception::what() const throw()
{
	return message;
}


General_exception::~General_exception() throw()
{

}
