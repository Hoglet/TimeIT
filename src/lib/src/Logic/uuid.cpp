//
// Created by hoglet on 11/11/2020.
//

#include <sstream>
#include <iomanip>
#include <utility>
#include <libtimeit/db/uuid.h>

namespace libtimeit
{


using namespace std;

bool rangeTestG4(const char &c)
{
	return  (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B');
}

bool is_valid_uuid(const string &uuid) // NOLINT(readability-function-cognitive-complexity)
{

	if (36 != uuid.length()) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	for (int position = 0; position < 8; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (isxdigit(uuid[position]) == 0)
		{
			return false;
		}
	}
	if ('-' != uuid[8])  // NOLINT
	{
		return false;
	}

	for (int position = 9; position < 13; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (isxdigit(uuid[position]) == 0)
		{
			return false;
		}
	}

	if ('-' != uuid[13])  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	for (int position = 14; position < 18; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (libtimeit::isxdigit(uuid[position]) == 0)
		{
			return false;
		}
	}

	if ('4' != uuid[14])// NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	if ('-' != uuid[18]) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	for (int position = 19; position < 23; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (libtimeit::isxdigit(uuid[position]) == 0)
		{
			return false;
		}
	}
	if ( ! rangeTestG4(uuid[19]))  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	if ('-' != uuid[23])  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	for (int position = 24; position < 36; position++) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if (libtimeit::isxdigit(uuid[position]) == 0)
		{
			return false;
		}
	}
	return true;
}

optional<UUID> UUID::from_string(string uuid)
{
	if (is_valid_uuid(uuid))
	{
		return UUID(uuid);
	}
	return {};
}

int random4hex()
{
	return (rand() % (0xFF + 1)) + (rand() % (0xFF + 1)); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

int randomBetween8and11()
{
	return (rand() % 4) + 8;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

UUID::UUID()
{
	stringstream uuid_string_stream;
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();
	uuid_string_stream << "-";
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();
	uuid_string_stream << "-4";
	uuid_string_stream << hex << setfill('0') << setw(3) << random4hex();
	uuid_string_stream << "-";
	uuid_string_stream << hex << randomBetween8and11();
	uuid_string_stream << hex << setfill('0') << setw(3) << random4hex();
	uuid_string_stream << "-";
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();
	uuid_string_stream << hex << setfill('0') << setw(4) << random4hex();

	uuid = uuid_string_stream.str();
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

UUID::UUID(string basic_string) : uuid(std::move(basic_string))
{
}

}
