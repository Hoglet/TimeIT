#ifndef TIMEIT_DATA_TYPES_H
#define TIMEIT_DATA_TYPES_H
#include <vector>
#include <cstdint>
#include <libtimeit/datatypes/task_id.h>

namespace libtimeit
{
using namespace std;

struct Time_entry;

using Time_list    = vector<Time_entry>;
using Task_id      = int64_t;
using task_id_list = vector<task_id>;

using Duration = long;      //Duration in seconds

}

#endif //TIMEIT_DATA_TYPES_H
