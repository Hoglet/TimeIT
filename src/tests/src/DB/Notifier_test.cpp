#include <gtest/gtest.h>
#include <libtimeit/db/notifier.h>
#include "notify_observer.h"

using namespace libtimeit;

namespace test
{

class NotifyTester : public Notify_observer
{
public:
	NotifyTester()
	{
		notifier.attach(this);
	}

	~NotifyTester()
	{
		notifier.detach(this);
	}

	Notifier notifier;
};


TEST( Notifier, TaskUpdated)
{
	NotifyTester test;
	test.notifier.send_notification(TASK_UPDATED, 1);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(1, test.task_id_updated);
}

TEST(Notifier, TaskAdded)
{
	NotifyTester test;
	test.notifier.send_notification(TASK_ADDED, 2);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(2, test.task_id_added);
}

TEST(Notifier,TaskRemoved)
{
	NotifyTester test;
	test.notifier.send_notification(TASK_REMOVED, 3);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(3, test.task_id_removed);
}
TEST(Notifier,TaskParentChanged)
{
	NotifyTester test;
	test.notifier.send_notification(TASK_PARENT_CHANGED, 4);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(4, test.task_id_parent);
}


}
