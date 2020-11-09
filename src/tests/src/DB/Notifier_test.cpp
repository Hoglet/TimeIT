#include "gtest/gtest.h"
#include "Notifier.h"

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
		task_id_name = 0;
		task_id_time = 0;
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

	virtual void on_taskNameChanged(int64_t t_id)
	{
		task_id_name = t_id;
	}

	virtual void on_taskTimeChanged(int64_t t_id)
	{
		task_id_time = t_id;
	}

	virtual void on_completeUpdate()
	{

	}

	Notifier notifier;
	int64_t task_id_updated;
	int64_t task_id_added;
	int64_t task_id_removed;
	int64_t task_id_parent;
	int64_t task_id_name;
	int64_t task_id_time;

};

TEST( Notifier, TaskUpdated)
{
	NotifyTester test;
	test.notifier.sendNotification(TASK_UPDATED, 1);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(1, test.task_id_updated);
}

TEST(Notifier, TaskAdded)
{
	NotifyTester test;
	test.notifier.sendNotification(TASK_ADDED, 2);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(2, test.task_id_added);
}

TEST(Notifier,TaskRemoved)
{
	NotifyTester test;
	test.notifier.sendNotification(TASK_REMOVED, 3);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(3, test.task_id_removed);
}
TEST(Notifier,TaskParentChanged)
{
	NotifyTester test;
	test.notifier.sendNotification(TASK_PARENT_CHANGED, 4);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(4, test.task_id_parent);
}


}
