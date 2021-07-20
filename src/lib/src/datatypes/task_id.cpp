#include "libtimeit/datatypes/task_id.h"

namespace libtimeit
{

task_id::task_id() noexcept: id(static_cast<string>(UUID()))
{}

task_id::task_id(const UUID& uid) noexcept: id(static_cast<string>(uid))
{}

task_id::operator string() const noexcept
{
	return id;
}

bool operator==(const task_id& id_1, const task_id& id_2)
{
	return 	static_cast<string>(id_1) == static_cast<string>(id_2);
}

bool operator!=(const task_id& id_1, const task_id& id_2)
{
	return !(id_1==id_2);
}


}