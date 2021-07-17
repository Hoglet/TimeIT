#ifndef TIMEIT_TIME_ID_H
#define TIMEIT_TIME_ID_H

#include <string>
#include <libtimeit/db/uuid.h>

namespace libtimeit
{
using namespace std;

class time_id
{
public:
	time_id();
	explicit time_id(const UUID&);
	explicit operator string() const;
private:
	const string id;
};

bool operator==(const time_id& id_1, const time_id& id_2);
bool operator!=(const time_id& id_1, const time_id& id_2);

}
#endif //TIMEIT_TIME_ID_H
