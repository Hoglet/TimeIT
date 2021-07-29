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

	void on_task_updated(const task_id& updated_task_id) override
	{
		task_id_updated = updated_task_id;
	}

	void on_task_added(const task& added_task) override
	{
		task_id_added = added_task.id;
	}

	void on_task_removed(const task& removed_task) override
	{
		task_id_removed = removed_task.id;
	}

	void on_parent_changed(const task& changed_task) override
	{
		task_id_parent = changed_task.id;
	}

	void on_task_name_changed(const task& changed_task) override
	{
		task_id_name = changed_task.id;
	}

	void on_task_time_changed(const task_id& changed_task) override
	{
		task_id_time = changed_task;
	}

	void on_complete_update() override
	{
		complete_update = true;
	}

	optional<task_id> task_id_updated;
	optional<task_id> task_id_added;
	optional<task_id> task_id_removed;
	optional<task_id> task_id_parent;
	optional<task_id> task_id_name;
	optional<task_id> task_id_time;
	bool complete_update = false;

};

}
#endif //TIMEIT_NOTIFY_OBSERVER_H
