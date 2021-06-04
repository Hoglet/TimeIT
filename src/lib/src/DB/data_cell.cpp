#include <libtimeit/db/data_cell.h>
#include <iostream>

namespace libtimeit
{

using namespace std;

const auto DATA_CELL_INTEGER = 1;
const auto DATA_CELL_TEXT    = 3;
const auto DATA_CELL_NULL    = 5;


Data_cell::Data_cell(int op_value) : type(DATA_CELL_INTEGER), value(op_value)
{
}

Data_cell::Data_cell(const char *op_text) :  type(DATA_CELL_TEXT), txt(op_text)
{
}

bool Data_cell::has_value() const
{
	return (type != DATA_CELL_NULL);
}

int64_t Data_cell::integer() const
{
	if (type != DATA_CELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return value;
}

string Data_cell::text() const
{
	if (type != DATA_CELL_TEXT)
	{

		throw db_exception("Wrong datatype on column");
	}
	return txt;
}

bool Data_cell::boolean() const
{
	if (type != DATA_CELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return (value != 0);
}

}

