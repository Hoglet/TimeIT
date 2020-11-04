#ifndef DATACELL_H_
#define DATACELL_H_

#include <string>
#include <dbexception.h>

namespace DBAbstraction
{

class DataCell
{
public:
	DataCell();
	DataCell(int);
	DataCell(const char *);
	virtual ~DataCell();
	bool hasValue();
	int getInt();
	std::string getString();
	bool getBool();
protected:
	int type;
	std::string text;
	int64_t value;
	dbexception dbe;
};

}

#endif /*DATACELL_H_*/
