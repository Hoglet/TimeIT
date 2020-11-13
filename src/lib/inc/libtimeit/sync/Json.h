#ifndef JSON_H_
#define JSON_H_

#include <string>
#include <vector>
#include <memory>
#include <libtimeit/db/Task.h>
#include <libtimeit/db/TimeAccessor.h>
#include <libtimeit/exceptions/GeneralException.h>

namespace Json
{
std::shared_ptr<std::vector<DB::Task>> toTasks(const std::string&);
std::vector<DB::TimeEntry> toTimes(const std::string&);

std::string toJson(std::shared_ptr<std::vector<DB::Task>>, std::string username);
std::string toJson(const std::vector<DB::TimeEntry>&);
}

#endif /* JSON_H_ */
