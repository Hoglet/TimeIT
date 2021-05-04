#ifndef TIMEIT_DATA_TYPES_H
#define TIMEIT_DATA_TYPES_H
#include <vector>
#include <cstdint>

namespace libtimeit
{
using namespace std;

struct Time_entry;

using Time_list    = vector<Time_entry>;
using Task_id      = int64_t;
using Time_id      = int64_t;
using Task_id_list = vector<Task_id>;

using Duration = int;      //Duration in seconds

}

#endif //TIMEIT_DATA_TYPES_H
