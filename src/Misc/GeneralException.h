/*
 * GeneralException.h
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#ifndef GENERALEXCEPTION_H_
#define GENERALEXCEPTION_H_

#include <exception>

class GeneralException: public std::exception
{
public:
	GeneralException();
	GeneralException(const char* message);
	virtual ~GeneralException() throw();
	virtual const char* what() const throw();
private:
	const char* message;
};

#endif /* GENERALEXCEPTION_H_ */
