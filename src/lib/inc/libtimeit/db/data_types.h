//
// Created by hoglet on 29/03/2021.
//

#ifndef TIMEIT_DATA_TYPES_H
#define TIMEIT_DATA_TYPES_H
#include <vector>

namespace libtimeit
{
using namespace std;

class Time_entry;

typedef vector<Time_entry> Time_list;
typedef int64_t            Task_ID;
typedef int64_t            Time_ID;
typedef vector<Task_ID>    Task_ID_list;

typedef int64_t            Duration;      //Duration in seconds

}

#endif //TIMEIT_DATA_TYPES_H
