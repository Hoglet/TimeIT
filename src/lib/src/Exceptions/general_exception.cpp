#include <libtimeit/exceptions/general_exception.h>

general_exception::general_exception(const char* op) : message(op)
{
}


const char* general_exception::what() const noexcept
{
	return message;
}


