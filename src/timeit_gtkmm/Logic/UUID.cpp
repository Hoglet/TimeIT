#include "UUID.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <Utils.h>


using namespace std;

void UUID::init()
{
	srand(Utils::now());
}

int UUID::random4hex()
{
	return (rand() % (0xFF + 1)) + (rand() % (0xFF + 1));
}

int UUID::randomBetween8and11()
{
	return (rand() % 4) + 8;
}

std::string UUID::randomUUID()
{
	//return "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
	stringstream uuid;
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	uuid << "-";
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	uuid << "-4";
	uuid << std::hex << setfill('0') << setw(3) << random4hex();
	uuid << "-";
	uuid << std::hex << randomBetween8and11();
	uuid << std::hex << setfill('0') << setw(3) << random4hex();
	uuid << "-";
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	uuid << std::hex << setfill('0') << setw(4) << random4hex();
	return uuid.str();
}

bool UUID::isValid(const std::string& uuid)
{

	if (36 != uuid.length())
	{
		return false;
	}

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	int position = 0;
	for (position = 0; position < 8; position++)
	{
		if (isHex(uuid[position]) == false)
		{
			return false;
		}
	}
	if ('-' != uuid[8])
	{
		return false;
	}

	for (position = 9; position < 13; position++)
	{
		if (isHex(uuid[position]) == false)
		{
			return false;
		}
	}

	if ('-' != uuid[13])
	{
		return false;
	}

	for (position = 14; position < 18; position++)
	{
		if (isHex(uuid[position]) == false)
		{
			return false;
		}
	}

	if ('4' != uuid[14])
	{
		return false;
	}

	if ('-' != uuid[18])
	{
		return false;
	}
	for (position = 19; position < 23; position++)
	{
		if (isHex(uuid[position]) == false)
		{
			return false;
		}
	}
	if (false == rangeTestG4(uuid[19]))
	{
		return false;
	}
	if ('-' != uuid[23])
	{
		return false;
	}
	for (position = 24; position < 36; position++)
	{
		if (isHex(uuid[position]) == false)
		{
			return false;
		}
	}
	return true;
}

bool UUID::isHex(const char& c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}
	if (c >= 'a' && c <= 'f')
	{
		return true;
	}
	if (c >= 'A' && c <= 'F')
	{
		return true;
	}
	return false;
}

bool UUID::rangeTestG4(const char& c)
{
	if (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B')
	{
		return true;
	}
	return false;
}
