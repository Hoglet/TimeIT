#ifndef JSON_H_
#define JSON_H_

#include <string>
#include <vector>
#include <memory>
#include <libtimeit/db/task.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/exceptions/general_exception.h>

namespace libtimeit
{
using namespace std;

vector<Task>       to_tasks(const string& json);
vector<Time_entry> to_times(const string& json);

string to_json(vector<Task> tasks, string username);
string to_json(const vector<Time_entry>& times);
}

#endif /* JSON_H_ */
