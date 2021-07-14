//
// Created by hoglet on 31/03/2021.
//

#ifndef TIMEIT_NOTIFY_OBSERVER_H
#define TIMEIT_NOTIFY_OBSERVER_H

#include <libtimeit/event_observer.h>

namespace test
{
using namespace libtimeit;

class Notify_observer : public event_observer
{
public:
	Notify_observer(notification_manager& notifier) : event_observer(notifier)
	{
	}

	virtual ~Notify_observer()
	{
	}

	virtual void on_task_updated(int64_t t_id)
	{
		task_id_updated = t_id;
	}

	virtual void on_task_added(int64_t t_id)
	{
		task_id_added = t_id;
	}

	virtual void on_task_removed(int64_t t_id)
	{
		task_id_removed = t_id;
	}

	virtual void on_parent_changed(int64_t t_id)
	{
		task_id_parent = t_id;
	}

	virtual void on_task_name_changed(int64_t t_id)
	{
		task_id_name = t_id;
	}

	virtual void on_task_time_changed(int64_t t_id)
	{
		task_id_time = t_id;
	}

	virtual void on_complete_update()
	{
		complete_update = true;
	}

	int64_t task_id_updated = 0;
	int64_t task_id_added = 0;
	int64_t task_id_removed = 0;
	int64_t task_id_parent = 0;
	int64_t task_id_name = 0;
	int64_t task_id_time = 0;
	bool complete_update = false;

};

}
#endif //TIMEIT_NOTIFY_OBSERVER_H
