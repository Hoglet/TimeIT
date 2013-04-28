#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Notifier_test.h"
#include "Notifier.h"
#include "ITaskAccessor.h"

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
	}

	virtual ~NotifyTester()
	{
	}

	void TestTaskUpdated()
	{
		notifier.attach(this);
		task_id_updated = 0;
		notifier.taskUpdated(1);
		ASSERT_EQUAL(1, task_id_updated);
	}

	void TestTaskAdded()
	{
		notifier.attach(this);
		task_id_added = 0;
		notifier.taskAdded(2);
		ASSERT_EQUAL(2, task_id_added);
	}

	void TestTaskRemoved()
	{
		notifier.attach(this);
		task_id_removed = 0;
		notifier.taskRemoved(3);
		ASSERT_EQUAL(3, task_id_removed);
	}
	void TestTaskParentChanged()
	{
		notifier.attach(this);
		task_id_parent = 0;
		notifier.taskParentChanged(4);
		ASSERT_EQUAL(4, task_id_parent);
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

private:
	Notifier notifier;
	int64_t task_id_updated;
	int64_t task_id_added;
	int64_t task_id_removed;
	int64_t task_id_parent;

};

void NotifyTaskUpdated()
{
	NotifyTester test;
	test.TestTaskUpdated();
}

void NotifyTaskAdded()
{
	NotifyTester test;
	test.TestTaskAdded();
}

void NotifyTaskRemoved()
{
	NotifyTester test;
	test.TestTaskRemoved();
}
void NotifyTaskParentChanged()
{
	NotifyTester test;
	test.TestTaskParentChanged();
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
