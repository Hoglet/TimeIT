/*
 * GeneralException.h
 *
 *  Created on: May 15, 2010
 *      Author: hoglet
 */

#ifndef GENERAL_EXCEPTION_H_
#define GENERAL_EXCEPTION_H_

#include <exception>
//LCOV_EXCL_START
class General_exception: public std::exception
{
public:
	General_exception(const char* message);
	[[nodiscard]] const char* what() const noexcept override;
private:
	const char* message;
};
//LCOV_EXCL_STOP

#endif /* GENERAL_EXCEPTION_H_ */
