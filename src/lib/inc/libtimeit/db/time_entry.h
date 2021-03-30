#ifndef TIMEIT_TIME_ENTRY_H
#define TIMEIT_TIME_ENTRY_H

#include <cstdint>
#include <string>
#include <libtimeit/db/data_types.h>
#include <libtimeit/db/uuid.h>

namespace libtimeit
{
using namespace std;

class Time_entry
{
public:
	Time_entry(
			Time_ID         id,
			UUID            uuid,
			Task_ID         task_ID,
			optional<UUID>  task_UUID,
			time_t          start,
			time_t          stop,
			bool            deleted,
			bool            running,
			time_t          changed);

	time_t               start()    const;
	time_t               stop()     const;
	Time_ID              ID()       const;
	Task_ID              task_ID()   const;
	class UUID           get_UUID() const;
	bool                 running()  const;
	time_t               changed()  const;
	bool                 deleted()  const;
	optional<class UUID> task_UUID() const;

	Time_entry with_start(time_t) const;
	Time_entry with_stop(time_t) const;
	Time_entry with_deleted(bool state) const;

	friend bool operator==(const Time_entry &op1, const Time_entry &op2);
	friend bool operator!=(const Time_entry &op1, const Time_entry &op2);

private:
	const int64_t              id_;
	class UUID           uuid_;
	int64_t              task_ID_;
	time_t               start_;
	time_t               stop_;
	bool                 deleted_;
	bool                 running_;
	time_t               changed_;
	optional<class UUID> task_UUID_;

	void operator=(const Time_entry&) {}
};
}
#endif