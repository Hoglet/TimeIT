#include "gtest/gtest.h"
#include "Task.h"

using namespace DB;

class TestTaskC
{
public:
	TestTaskC()
	{
	}
	void testValidUUID()
	{
		Task task("Name", 0, "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d", false, 0, 0, "", false);
	}
	void testInvalidUUID()
	{
		Task task("Name", 0, "ascd", false, 0, 0, "", false);
	}
	void testInvalidParentUUID()
	{
		Task task("Name", 0, "73cf62ec-afc6-4a72-95a3-93a5b9f10b2d", false, 0, 0, "abcd", false);
	}
};

TEST(Task, creation)
{
	TestTaskC test_object;
	ASSERT_NO_THROW(test_object.testValidUUID());
	ASSERT_THROW(test_object.testInvalidUUID(), std::invalid_argument);
	ASSERT_THROW(test_object.testInvalidParentUUID(), std::invalid_argument);
}

TEST( Task, getters)
{
	Task task("Name");
	ASSERT_EQ(0, task.getID());
	ASSERT_EQ(0, task.getParentID());

	ASSERT_EQ("Name", task.getName());
	ASSERT_EQ("", task.getUUID());
	ASSERT_EQ(false, task.getCompleted());

}

TEST (Task, with)
{
	Task original("Apa", 0, "", false, 0, 0, "", false);
	auto now = time(0);

	Task task = original.withName("Test");
	ASSERT_EQ("Test", task.getName());

	task = original.withParent(13);
	ASSERT_EQ(13, task.getParentID());

	task = original.withCompleted(true);
	ASSERT_EQ(true, task.getCompleted());

	task = original.withDeleted(true);
	ASSERT_EQ(true, task.getDeleted());

	auto lastChanged = task.getLastChanged();
	ASSERT_TRUE(abs(lastChanged - now) < 2);

}

