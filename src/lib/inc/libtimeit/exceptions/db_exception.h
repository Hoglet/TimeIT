#ifndef DB_EXCEPTION_H
#define DB_EXCEPTION_H

#include <iostream>
#include <string>
#include <exception>
namespace libtimeit
{
using namespace std;

class db_exception : public std::exception
{
public:
	db_exception( string_view op_message, int op_return_code = 0);

	[[nodiscard]] const char *what() const noexcept override;
private:
	string message;
	int    return_code;

	[[nodiscard]] string rc_to_string() const;
};

}
#endif /*DB_EXCEPTION_H*/
