//
// Created by hoglet on 11/11/2020.
//
#ifndef TIMEIT_UUID_H
#define TIMEIT_UUID_H

#include <optional>

namespace libtimeit
{
using namespace std;

class UUIDTool;

class UUID;

optional<UUID> toUuid(string uuid);

class UUID
{
	friend optional<UUID> toUuid(string uuid);
	friend UUIDTool;
public:
	UUID();
	bool operator==(const UUID &rhs) const;
	bool operator!=(const UUID &rhs) const;
	const char *c_str() const;
private:
	string uuid;
	void operator=(const UUID &);
	UUID(string basicString);
};

}
#endif