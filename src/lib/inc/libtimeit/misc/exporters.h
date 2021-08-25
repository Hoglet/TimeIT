#ifndef EXPORTERS_H
#define EXPORTERS_H

#include <libtimeit/db/database.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/task_accessor.h>
#include <chrono>

namespace libtimeit
{

using namespace std;
using namespace std::chrono;

bool export_to_csv( database& db , const string &filename, time_point<system_clock> start, time_point<system_clock> stop );


}

#endif //EXPORTERS_H
