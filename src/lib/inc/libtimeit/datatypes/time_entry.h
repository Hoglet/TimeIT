#ifndef TIME_ENTRY_H
#define TIME_ENTRY_H

#include <cstdint>
#include <string>
#include <libtimeit/db/data_types.h>
#include <libtimeit/datatypes/time_id.h>


namespace libtimeit
{
using namespace std;

enum time_entry_state
{
	STOPPED = 0,
	RUNNING = 1,
	PAUSED  = 2,
	DELETED = 3
};

struct time_entry
{
	const time_id              id;
	const time_t               start;
	const time_t               stop;
	const time_entry_state     state;
	const time_t               changed;
	const task_id              owner_id;
	const string               comment;

	time_entry(
			task_id          owner,
			time_t           op_start,
			time_t           op_stop
	);
	time_entry(
			task_id          owner_id,
			time_t           op_start,
			time_t           op_stop,
			time_entry_state op_state
	);

	time_entry(
			time_id           op_id,
			task_id           owner_id,
			time_t            op_start,
			time_t            op_stop,
			time_entry_state  op_state,
			time_t            op_changed,
			string            op_comment);

	[[nodiscard]] time_entry with_start( time_t) const;
	[[nodiscard]] time_entry with_stop( time_t) const;
	[[nodiscard]] time_entry with( time_entry_state ) const;
	[[nodiscard]] time_entry with_comment( string comment);
};
bool operator==( const time_entry &op_1, const time_entry &op_2);
bool operator!=( const time_entry &op_1, const time_entry &op_2);

}
#endif