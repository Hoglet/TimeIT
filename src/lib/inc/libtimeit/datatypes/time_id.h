#ifndef TIMEIT_TIME_ID_H
#define TIMEIT_TIME_ID_H

#include <string>
#include <libtimeit/datatypes/uuid.h>

namespace libtimeit
{
using namespace std;

class time_id
{
public:
	time_id() noexcept;
	explicit time_id(const uuid&) noexcept;
	explicit operator string() const noexcept;
private:
	string id;
};

bool operator==(const time_id& id_1, const time_id& id_2);
bool operator!=(const time_id& id_1, const time_id& id_2);

}
#endif //TIMEIT_TIME_ID_H
