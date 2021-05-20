#include <gtest/gtest.h>
#include <libtimeit/db/notifier.h>
#include "notify_observer.h"

using namespace libtimeit;

namespace test
{




TEST( Notifier, TaskUpdated)
{
	Notifier notifier;
	Notify_observer test(notifier);
	notifier.try_send_notification(TASK_UPDATED, 1);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(1, test.task_id_updated);
}

TEST(Notifier, TaskAdded)
{
	Notifier notifier;
	Notify_observer test(notifier);
	notifier.try_send_notification(TASK_ADDED, 2);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(2, test.task_id_added);
}

TEST(Notifier,TaskRemoved)
{
	Notifier notifier;
	Notify_observer test(notifier);
	notifier.try_send_notification(TASK_REMOVED, 3);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_parent);
	ASSERT_EQ(3, test.task_id_removed);
}
TEST(Notifier,TaskParentChanged)
{
	Notifier notifier;
	Notify_observer test(notifier);
	notifier.try_send_notification(TASK_PARENT_CHANGED, 4);
	ASSERT_EQ(0, test.task_id_updated);
	ASSERT_EQ(0, test.task_id_added);
	ASSERT_EQ(0, test.task_id_removed);
	ASSERT_EQ(4, test.task_id_parent);
}


}
