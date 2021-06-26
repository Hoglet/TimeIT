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
	const Time_id              id;
	const class UUID           uuid;
	const Task_id              task_id;
	const time_t               start;
	const time_t               stop;
	const Time_entry_state     state;
	const time_t               changed;
	const optional<class UUID> task_uuid = {};
	const string               comment;

	Time_entry(
			Task_id          task_id,
			time_t           start,
			time_t           stop
			);
	Time_entry(
			Task_id          task_id,
			time_t           start,
			time_t           stop,
			Time_entry_state state
	);

	Time_entry(
			Time_id          id,
			UUID             uuid,
			Task_id          task_id,
			optional<UUID>   task_uuid,
			time_t           start,
			time_t           stop,
			Time_entry_state state,
			time_t           changed,
			string           comment);

	[[nodiscard]] Time_entry with_start(time_t) const;
	[[nodiscard]] Time_entry with_stop(time_t) const;
	[[nodiscard]] Time_entry with( Time_entry_state ) const;
	[[nodiscard]] Time_entry with_comment(string comment);
};
bool operator==(const Time_entry &op1, const Time_entry &op2);
bool operator!=(const Time_entry &op1, const Time_entry &op2);

}
#endif