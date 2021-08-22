#ifndef GENERAL_EXCEPTION_H_
#define GENERAL_EXCEPTION_H_

#include <exception>
//LCOV_EXCL_START
class general_exception: public std::exception
{
public:
	general_exception(const char* message);
	[[nodiscard]] const char* what() const noexcept override;
private:
	const char* message;
};
//LCOV_EXCL_STOP

#endif /* GENERAL_EXCEPTION_H_ */
