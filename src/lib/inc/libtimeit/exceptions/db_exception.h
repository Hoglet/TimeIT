#ifndef DBEXCEPTION_H
#define DBEXCEPTION_H

#include <iostream>
#include <string>
#include <exception>
namespace libtimeit
{
using namespace std;

class db_exception : public std::exception
{
public:
	db_exception(string message, int return_code = 0);
	virtual ~db_exception() throw();

	virtual const char *what() const throw();
private:
	string message;
	int    return_code;
	string rc_to_string() const;
};

}
#endif /*DBEXCEPTION_H*/
