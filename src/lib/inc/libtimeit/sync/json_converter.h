#ifndef JSON_CONVERTER_H
#define JSON_CONVERTER_H

#include <string>
#include <vector>
#include <memory>
#include <libtimeit/datatypes/task.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/exceptions/general_exception.h>

namespace libtimeit
{
using namespace std;

vector<task>       to_tasks(const string& json);
vector<time_entry> to_times( const string& json);

string to_json(const vector<task>& tasks, const string& username);
string to_json(const vector<time_entry>& times);
}

#endif /* JSON_CONVERTER_H */
