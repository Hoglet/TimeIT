#ifndef DATA_CELL_H_
#define DATA_CELL_H_

#include <string>
#include <libtimeit/exceptions/db_exception.h>

namespace libtimeit
{
using namespace std;

class Data_cell
{
public:
	Data_cell();
	Data_cell(int);
	Data_cell(const char*);
	virtual ~Data_cell();

	bool     has_value();
	int64_t  integer();
	string   text();
	bool     boolean();
protected:
	int      type;
	string   txt;
	int64_t  value;
};

}

#endif /*DATA_CELL_H_*/
