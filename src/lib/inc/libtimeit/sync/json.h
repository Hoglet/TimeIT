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

vector<Task>      toTasks(const string&);
vector<Time_entry> toTimes(const string&);

string toJson(vector<Task>, string username);
string toJson(const vector<Time_entry>&);
}

#endif /* JSON_H_ */
