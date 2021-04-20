//
// Created by hoglet on 11/11/2020.
//
#ifndef TIMEIT_UUID_H
#define TIMEIT_UUID_H

#include <optional>

namespace libtimeit
{
using namespace std;

class UUID
{
public:
	UUID();
	static optional<UUID> from_string(string uuid);

	bool operator==(const UUID &rhs) const;
	bool operator!=(const UUID &rhs) const;
	const char *c_str() const;
private:
	string uuid;
	void operator=(const UUID &);
	UUID(string basic_string);
};

}
#endif