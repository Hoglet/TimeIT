#pragma once

#include <cstdint>
#include <string>
#include "UUID.h"

namespace DB
{
using std::string;
using std::optional;
class TimeEntry
{
public:
	TimeEntry(
			int64_t         id,
			UUID            uuid_,
			int64_t         taskID,
			optional<UUID>  taskUUID,
			time_t          start,
			time_t          stop,
			bool            deleted,
			bool            running,
			time_t          changed);

	time_t               start()    const;
	time_t               stop()     const;
	int64_t              ID()       const;
	int64_t              taskID()   const;
	class UUID           getUUID()     const;
	bool                 running()  const;
	time_t               changed()  const;
	bool                 deleted()  const;
	optional<class UUID> taskUUID() const;

	TimeEntry withStart(time_t) const;
	TimeEntry withStop(time_t) const;
	TimeEntry withDeleted(bool state) const;

	friend bool operator==(const TimeEntry &op1, const TimeEntry &op2);
	friend bool operator!=(const TimeEntry &op1, const TimeEntry &op2);

private:
	int64_t              id_;
	class UUID           uuid_;
	int64_t              taskID_;
	time_t               start_;
	time_t               stop_;
	bool                 deleted_;
	bool                 running_;
	time_t               changed_;
	optional<class UUID> taskUUID_;

	void operator=(const TimeEntry&) {}
};
}
