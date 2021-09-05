#ifndef TASK_ID_H
#define TASK_ID_H

#include <string>
#include <libtimeit/datatypes/uuid.h>
#include <vector>

namespace libtimeit
{
using namespace std;

class task_id
{
public:
	task_id() noexcept;
	explicit task_id(const uuid&) noexcept;
	explicit operator string() const noexcept;
private:
	string id;
};

bool operator==(const task_id& id_1, const task_id& id_2);
bool operator!=(const task_id& id_1, const task_id& id_2);

using task_id_list = vector<task_id>;

}
#endif //TASK_ID_H
