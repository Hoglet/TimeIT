//
// Created by hoglet on 11/11/2020.
//

#include <sstream>
#include <iomanip>
#include <utility>
#include <libtimeit/datatypes/uuid.h>

namespace libtimeit
{


using namespace std;

constexpr bool rangeTestG4(const char &c)
{
	return  (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B');
}

constexpr bool is_valid_uuid( string_view id ) // NOLINT(readability-function-cognitive-complexity)
{

	if ( 36 != id.length()) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	for (int position = 0; position < 8; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if ( isxdigit( id[position]) == 0)
		{
			return false;
		}
	}
	if ( '-' != id[8])  // NOLINT
	{
		return false;
	}

	for (int position = 9; position < 13; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if ( isxdigit( id[position]) == 0)
		{
			return false;
		}
	}

	if ( '-' != id[13])  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	for (int position = 14; position < 18; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if ( libtimeit::isxdigit( id[position]) == 0)
		{
			return false;
		}
	}

	if ( '4' != id[14])// NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}

	if ( '-' != id[18]) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	for (int position = 19; position < 23; position++)  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if ( libtimeit::isxdigit( id[position]) == 0)
		{
			return false;
		}
	}
	if ( ! rangeTestG4( id[19]))  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	if ( '-' != id[23])  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		return false;
	}
	for (int position = 24; position < 36; position++) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		if ( libtimeit::isxdigit( id[position]) == 0)
		{
			return false;
		}
	}
	return true;
}

optional<uuid> uuid::from_string( string id)
{
	if (is_valid_uuid( id))
	{
		return uuid( id);
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

uuid::uuid()
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

	id = uuid_string_stream.str();
}

bool uuid::operator==( const uuid &rhs) const
{
	return id == rhs.id;
}

bool uuid::operator!=( const uuid &rhs) const
{
	return !(rhs == *this);
}

const char *uuid::c_str() const
{
	return id.c_str();
}

string uuid::to_string() const
{
	return id;
}

uuid::uuid( string basic_string) : id( std::move( basic_string))
{
}

uuid::operator std::string() const
{
	return id;
}

}
