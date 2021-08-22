#include "libtimeit/datatypes/time_id.h"

namespace libtimeit
{

time_id::time_id() noexcept: id(static_cast<string>(uuid()))
{}

time_id::time_id(const uuid& uid) noexcept: id( static_cast<string>(uid))
{}

time_id::operator string() const noexcept
{
	return id;
}

bool operator==(const time_id& id_1, const time_id& id_2)
{
	return 	static_cast<string>(id_1) == static_cast<string>(id_2);
}

bool operator!=(const time_id& id_1, const time_id& id_2)
{
	return !(id_1==id_2);
}


}