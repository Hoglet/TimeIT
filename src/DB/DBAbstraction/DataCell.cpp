#include "DataCell.h"
#include <sstream>
#include <iostream>
using namespace std;

namespace DBAbstraction
{

DataCell::DataCell(const char * op)
{
	if(op)
	{
		data=op;
	}
}
DataCell::DataCell(const DataCell& op)
{
	if (&op!=this)
	{
		data=op.data;
	}
}

bool DataCell::isNull()
{
	return data.length()<1;
}

int DataCell::getInt()
{
	stringstream str(data);
	int retVal;
	if((str>>retVal).fail())
	{
		e.setReturnCode(0);
		e.setMessage("Not a number");
		throw e;
	}
	return retVal;
}
bool DataCell::getBool()
{
	bool retVal;
	if(data=="true" || data=="1")
	{
		retVal=true;
	}
	else
	{
		retVal=false;
	}
	return retVal;
}

std::string DataCell::getString()
{
	return data;
}

DataCell& DataCell::operator= (const DataCell& op1)
{
	data=op1.data;
	return *this;
}

DataCell::DataCell()
{
}

DataCell::~DataCell()
{
}

}
