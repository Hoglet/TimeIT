#include <libtimeit/db/data_cell.h>
#include <iostream>

namespace libtimeit
{

using namespace std;

const auto data_cell_integer = 1;
const auto data_cell_text    = 3;
const auto data_cell_null    = 5;

data_cell::data_cell(int op_value) : type( data_cell_integer), value( op_value)
{
}

data_cell::data_cell(const char *op_text) : type( data_cell_text), txt( op_text)
{
}

bool data_cell::has_value() const
{
	return ( type != data_cell_null);
}

int64_t data_cell::integer() const
{
	if ( type != data_cell_integer)
	{
		throw db_exception("Wrong datatype on column");
	}
	return value;
}

string data_cell::text() const
{
	if ( type == data_cell_null )
	{
		return "";
	}
	if ( type != data_cell_text)
	{

		throw db_exception("Wrong datatype on column");
	}
	return txt;
}

bool data_cell::boolean() const
{
	if ( type != data_cell_integer)
	{
		throw db_exception("Wrong datatype on column");
	}
	return (value != 0);
}

}
