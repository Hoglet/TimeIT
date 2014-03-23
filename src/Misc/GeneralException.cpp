/*
 * GeneralException.cpp
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#include "GeneralException.h"

GeneralException::GeneralException()
{
	message = 0;
}

void GeneralException::setMessage(const char* op1)
{
	message = op1;
}

const char* GeneralException::what() const throw()
{
	return message;
}


GeneralException::~GeneralException() throw()
{

}
