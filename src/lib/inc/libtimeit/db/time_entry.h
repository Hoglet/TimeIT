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
	STOPPED,
	RUNNING,
	PAUSED,
	DELETED
};

struct Time_entry
{
	const Time_ID              ID;
	const class UUID           uuid;
	const Task_ID              task_ID;
	const time_t               start;
	const time_t               stop;
	const Time_entry_state     state;
	const time_t               changed;
	const optional<class UUID> task_uuid;

	Time_entry(
			Task_ID task_ID_,
			time_t  start_,
			time_t  stop_
			);
	Time_entry(
			Task_ID          task_ID_,
			time_t           start_,
			time_t           stop_,
			Time_entry_state state_
	);

	Time_entry(
			Time_ID          id_,
			UUID             uuid_,
			Task_ID          task_ID_,
			optional<UUID>   taskUUID_,
			time_t           start_,
			time_t           stop_,
			Time_entry_state state_,
			time_t           changed_);


	bool                 running()  const;

	Time_entry with_start(time_t) const;
	Time_entry with_stop(time_t) const;
	Time_entry with( Time_entry_state ) const;

	friend bool operator==(const Time_entry &op1, const Time_entry &op2);
	friend bool operator!=(const Time_entry &op1, const Time_entry &op2);
};
}
#endif