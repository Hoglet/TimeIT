//
// Created by hoglet on 11/11/2020.
//

#include <sstream>
#include <iomanip>
#include "DB/UUID.h"

using namespace std;

bool rangeTestG4(const char& c)
{
	if (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B')
	{
		return true;
	}
	return false;
}

bool isValid(const std::string& uuid)
{

	if (36 != uuid.length())
	{
		return false;
	}

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	int position = 0;
	for (position = 0; position < 8; position++)
	{
		if (isxdigit(uuid[position]) == false)
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
		if (isxdigit(uuid[position]) == false)
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
		if (isxdigit(uuid[position]) == false)
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
		if (isxdigit(uuid[position]) == false)
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
		if (isxdigit(uuid[position]) == false)
		{
			return false;
		}
	}
	return true;
}





std::optional<UUID> toUuid(std::string uuid)
{
	if (isValid(uuid))
	{
		return UUID(uuid);
	}
	return {};
}

int random4hex()
{
	return (rand() % (0xFF + 1)) + (rand() % (0xFF + 1));
}

int randomBetween8and11()
{
	return (rand() % 4) + 8;
}

UUID::UUID()
{
	stringstream uuidSStream;
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();
	uuidSStream << "-";
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();
	uuidSStream << "-4";
	uuidSStream << std::hex << setfill('0') << setw(3) << random4hex();
	uuidSStream << "-";
	uuidSStream << std::hex << randomBetween8and11();
	uuidSStream << std::hex << setfill('0') << setw(3) << random4hex();
	uuidSStream << "-";
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();
	uuidSStream << std::hex << setfill('0') << setw(4) << random4hex();

	uuid=uuidSStream.str();
}

bool UUID::operator==(const UUID &rhs) const
{
	return uuid == rhs.uuid;
}

bool UUID::operator!=(const UUID &rhs) const
{
	return !(rhs == *this);
}

const char *UUID::c_str() const
{
	return uuid.c_str();
}

UUID::UUID(std::string op_uuid)
{
	uuid=op_uuid;
}
