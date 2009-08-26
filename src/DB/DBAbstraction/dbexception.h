#ifndef DBEXCEPTION_H_
#define DBEXCEPTION_H_
#include <iostream>
#include <string>
#include <exception>

class dbexception : public std::exception
{
public:
	dbexception();
	virtual ~dbexception() throw();
	virtual const char* what() const throw();
	void setMessage(std::string op1);
	void setReturnCode(int op1);
private:
	std::string message;
	int returnCode;
	const char* rc2string() const;
};

#endif /*DBEXCEPTION_H_*/
