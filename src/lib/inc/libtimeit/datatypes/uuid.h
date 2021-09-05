//
// Created by hoglet on 11/11/2020.
//
#ifndef UUID_H
#define UUID_H

#include <optional>

namespace libtimeit
{
using namespace std;

class uuid
{
public:
	uuid();
	static optional<uuid> from_string( string /*id_string*/);

	bool operator==(const uuid &rhs) const;
	bool operator!=(const uuid &rhs) const;
	[[nodiscard]] const char *c_str() const;
	[[nodiscard]] string to_string() const;
	explicit operator std::string() const;
private:
	string id;
	explicit uuid( string basic_string);
};


}
#endif