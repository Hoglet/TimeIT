#ifndef DATACELL_H_
#define DATACELL_H_

#include <string>
#include <dbexception.h>

namespace DBAbstraction
{

class DataCell
{
public:
	DataCell(const char*);
	DataCell(const DataCell&);
	int getInt();
	bool isNull();
	std::string getString();
	bool getBool();
	virtual ~DataCell();
	DataCell();
	DataCell& operator= (const DataCell&);
private:
	std::string data;
	dbexception e;
};

}

#endif /*DATACELL_H_*/
