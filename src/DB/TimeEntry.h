
#ifndef TIMEENTRY_H_
#define TIMEENTRY_H_

#include <stdint.h>
#include <string>

namespace DB
{

class TimeEntry
{
public:
	TimeEntry(	int64_t id, std::string uuid, int64_t taskID, std::string taskUUID, time_t start, time_t stop, bool deleted, bool running, int64_t changed);
	time_t getStart() const;
	void setStart(time_t);
	time_t getStop() const;
	void setStop(time_t);
	int64_t getID() const;
	int64_t getTaskID() const;
	std::string getUUID() const;
	bool getRunning() const;
	time_t getLastChanged() const;
	bool getDeleted() const;
	void setDeleted(bool state);
	std::string getTaskUUID() const;
	friend bool operator== (const TimeEntry &op1, const TimeEntry &op2);
	friend bool operator!= (const TimeEntry &op1, const TimeEntry &op2);
private:
	int64_t id;
	std::string uuid;
	int64_t taskID;
	time_t start;
	time_t stop;
	bool deleted;
	bool running;
	time_t changed;
	std::string taskUUID;
};
}

#endif /* TIMEENTRY_H_ */
