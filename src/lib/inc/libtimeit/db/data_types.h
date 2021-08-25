#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#include <vector>
#include <cstdint>
#include <libtimeit/datatypes/task_id.h>

namespace libtimeit
{
using namespace std;

struct time_entry;

using time_list    = vector<time_entry>;
using Task_id      = int64_t;
using task_id_list = vector<task_id>;

using Duration = long;      //Duration in seconds

}

#endif //DATA_TYPES_H
