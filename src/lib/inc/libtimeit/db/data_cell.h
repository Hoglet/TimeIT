#ifndef DATA_CELL_H_
#define DATA_CELL_H_

#include <string>
#include <libtimeit/exceptions/db_exception.h>

namespace libtimeit
{
using namespace std;

struct Data_cell
{
public:
	Data_cell() = default;
	Data_cell(int);
	Data_cell(const char*);

	[[nodiscard]] bool     has_value() const;
	[[nodiscard]] int64_t  integer()   const;
	[[nodiscard]] string   text()      const;
	[[nodiscard]] bool     boolean()   const;
private:
	const int      type{5};
	const string   txt;
	const int64_t  value{0};
};

}

#endif /*DATA_CELL_H_*/
