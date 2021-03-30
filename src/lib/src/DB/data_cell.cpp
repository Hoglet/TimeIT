#include <libtimeit/db/data_cell.h>
#include <sstream>
#include <iostream>

namespace libtimeit
{

using namespace std;

constexpr auto DATACELL_INTEGER = 1;
constexpr auto DATACELL_FLOAT = 2;
constexpr auto DATACELL_TEXT = 3;
constexpr auto DATACELL_BLOB = 4;
constexpr auto DATACELL_NULL = 5;
constexpr auto DATACELL_BOOL = 6;


Data_cell::Data_cell()
{
	value = 0;
	type = DATACELL_NULL;
}

Data_cell::Data_cell(int op_value)
{
	value = op_value;
	type = DATACELL_INTEGER;
}

Data_cell::Data_cell(const char *op_text) : txt(op_text)
{
	type = DATACELL_TEXT;
	value = 0;
}


Data_cell::~Data_cell()
{
}

bool Data_cell::has_value()
{
	return (type != DATACELL_NULL);
}

int64_t Data_cell::integer()
{
	if (type != DATACELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return value;
}

string Data_cell::text()
{
	if (type != DATACELL_TEXT)
	{

		throw db_exception("Wrong datatype on column");
	}
	return txt;
}

bool Data_cell::boolean()
{
	if (type != DATACELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return (value != 0);
}

}

