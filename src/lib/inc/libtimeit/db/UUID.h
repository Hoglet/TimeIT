//
// Created by hoglet on 11/11/2020.
//
#pragma once

#include <optional>

class UUIDTool;
class UUID;
std::optional<UUID> toUuid(std::string uuid);

class UUID
{
	friend std::optional<UUID> toUuid(std::string uuid);
	friend UUIDTool;
public:
	UUID();
	bool operator==(const UUID &rhs) const;
	bool operator!=(const UUID &rhs) const;
	const char* c_str() const;
private:
	std::string uuid;
	void operator=(const UUID&);
	UUID(std::string basicString);
};


