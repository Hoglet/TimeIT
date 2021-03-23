#ifndef JSON_H_
#define JSON_H_

#include <string>
#include <vector>
#include <memory>
#include <libtimeit/db/Task.h>
#include <libtimeit/db/TimeAccessor.h>
#include <libtimeit/exceptions/GeneralException.h>

namespace libtimeit
{
using namespace std;

vector<Task>      toTasks(const string&);
vector<TimeEntry> toTimes(const string&);

string toJson(vector<Task>, string username);
string toJson(const vector<TimeEntry>&);
}

#endif /* JSON_H_ */
