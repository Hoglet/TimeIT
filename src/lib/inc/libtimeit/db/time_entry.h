#ifndef TIMEIT_TIME_ENTRY_H
#define TIMEIT_TIME_ENTRY_H

#include <cstdint>
#include <string>
#include <libtimeit/db/data_types.h>
#include <libtimeit/db/uuid.h>

namespace libtimeit
{
using namespace std;

enum Time_entry_state
{
	STOPPED = 0,
	RUNNING = 1,
	PAUSED  = 2,
	DELETED = 3
};

struct Time_entry
{
	const Time_id              ID;
	const class UUID           uuid;
	const Task_id              task_ID;
	const time_t               start;
	const time_t               stop;
	const Time_entry_state     state;
	const time_t               changed;
	const optional<class UUID> task_uuid;

	Time_entry(
			Task_id task_ID_,
			time_t  start_,
			time_t  stop_
			);
	Time_entry(
			Task_id          task_ID_,
			time_t           start_,
			time_t           stop_,
			Time_entry_state state_
	);

	Time_entry(
			Time_id          id_,
			UUID             uuid_,
			Task_id          task_ID_,
			optional<UUID>   taskUUID_,
			time_t           start_,
			time_t           stop_,
			Time_entry_state state_,
			time_t           changed_);

	[[nodiscard]] Time_entry with_start(time_t) const;
	[[nodiscard]] Time_entry with_stop(time_t) const;
	[[nodiscard]] Time_entry with( Time_entry_state ) const;

	friend bool operator==(const Time_entry &op1, const Time_entry &op2);
	friend bool operator!=(const Time_entry &op1, const Time_entry &op2);
};
}
#endif