#include <libtimeit/db/data_cell.h>
#include <iostream>

namespace libtimeit
{

using namespace std;

const auto DATA_CELL_INTEGER = 1;
const auto DATA_CELL_TEXT    = 3;
const auto DATA_CELL_NULL    = 5;

data_cell::data_cell(int op_value) : type(DATA_CELL_INTEGER), value(op_value)
{
}

data_cell::data_cell(const char *op_text) : type(DATA_CELL_TEXT), txt(op_text)
{
}

bool data_cell::has_value() const
{
	return (type != DATA_CELL_NULL);
}

int64_t data_cell::integer() const
{
	if (type != DATA_CELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return value;
}

string data_cell::text() const
{
	if (type == DATA_CELL_NULL )
	{
		return "";
	}
	if (type != DATA_CELL_TEXT)
	{

		throw db_exception("Wrong datatype on column");
	}
	return txt;
}

bool data_cell::boolean() const
{
	if (type != DATA_CELL_INTEGER)
	{
		throw db_exception("Wrong datatype on column");
	}
	return (value != 0);
}

}

