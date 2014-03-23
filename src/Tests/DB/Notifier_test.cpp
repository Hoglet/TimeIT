#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Notifier_test.h"
#include "Notifier.h"
#include "ExtendedTaskAccessor.h"

using namespace DB;
namespace Test
{

class NotifyTester: public TaskAccessorObserver
{
public:
	NotifyTester()
	{
		task_id_added = 0;
		task_id_removed = 0;
		task_id_updated = 0;
		task_id_parent = 0;
		notifier.attach(this);
	}

	virtual ~NotifyTester()
	{
	}

	virtual void on_taskUpdated(int64_t t_id)
	{
		task_id_updated = t_id;
	}

	virtual void on_taskAdded(int64_t t_id)
	{
		task_id_added = t_id;
	}

	virtual void on_taskRemoved(int64_t t_id)
	{
		task_id_removed = t_id;
	}

	virtual void on_taskParentChanged(int64_t t_id)
	{
		task_id_parent = t_id;
	}

	virtual void on_completeUpdate()
	{

	}

	Notifier notifier;
	int64_t task_id_updated;
	int64_t task_id_added;
	int64_t task_id_removed;
	int64_t task_id_parent;

};

void NotifyTaskUpdated()
{
	NotifyTester test;
	test.notifier.taskUpdated(1);
	ASSERT_EQUAL(1, test.task_id_updated);
}

void NotifyTaskAdded()
{
	NotifyTester test;
	test.notifier.taskAdded(2);
	ASSERT_EQUAL(2, test.task_id_added);
}

void NotifyTaskRemoved()
{
	NotifyTester test;
	test.notifier.taskRemoved(3);
	ASSERT_EQUAL(3, test.task_id_removed);
}
void NotifyTaskParentChanged()
{
	NotifyTester test;
	test.notifier.taskParentChanged(4);
	ASSERT_EQUAL(4, test.task_id_parent);
}




cute::suite make_suite_Notifier_test()
{
	cute::suite s;
	s.push_back(CUTE(NotifyTaskUpdated));
	s.push_back(CUTE(NotifyTaskAdded));
	s.push_back(CUTE(NotifyTaskRemoved));
	s.push_back(CUTE(NotifyTaskParentChanged));
	return s;
}

}
