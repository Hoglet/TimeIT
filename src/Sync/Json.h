#ifndef JSON_H_
#define JSON_H_

#include <string>
#include "Task.h"
#include "TimeAccessor.h"
#include "GeneralException.h"
#include <vector>
#include <memory>

class Json
{
public:
	Json();
	virtual ~Json();
	std::shared_ptr<std::vector<DB::Task>> toTasks(const std::string&);
	std::shared_ptr<std::vector<DB::TimeEntry> > toTimes(const std::string&);

	std::string toJson(std::shared_ptr<std::vector<DB::Task>>);
	std::string toJson(std::shared_ptr<std::vector<DB::TimeEntry>>);
private:
	GeneralException ge;
};

#endif /* JSON_H_ */
