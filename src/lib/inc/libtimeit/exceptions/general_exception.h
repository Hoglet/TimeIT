/*
 * GeneralException.h
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#ifndef GENERALEXCEPTION_H_
#define GENERALEXCEPTION_H_

#include <exception>
//LCOV_EXCL_START
class General_exception: public std::exception
{
public:
	General_exception();
	void setMessage(const char* message);
	virtual ~General_exception() throw();
	virtual const char* what() const throw();
private:
	const char* message;
};
//LCOV_EXCL_STOP

#endif /* GENERALEXCEPTION_H_ */
