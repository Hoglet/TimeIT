#ifndef JSON_H_
#define JSON_H_

#include <string>
#include "Task.h"
#include "TimeAccessor.h"
#include "GeneralException.h"
#include <vector>
#include <memory>

namespace Json
{
std::shared_ptr<std::vector<DB::Task>> toTasks(const std::string&);
std::vector<DB::TimeEntry> toTimes(const std::string&);

std::string toJson(std::shared_ptr<std::vector<DB::Task>>, std::string username);
std::string toJson(const std::vector<DB::TimeEntry>&);
}

#endif /* JSON_H_ */
