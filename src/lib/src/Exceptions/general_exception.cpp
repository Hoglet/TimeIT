/*
 * GeneralException.cpp
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#include <libtimeit/exceptions/general_exception.h>

General_exception::General_exception(const char* op1) : message(op1)
{
}


const char* General_exception::what() const noexcept
{
	return message;
}


