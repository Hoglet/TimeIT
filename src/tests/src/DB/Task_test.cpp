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
	ASSERT_EQ(0, task.ID());
	ASSERT_EQ(0, task.parentID());

	ASSERT_EQ("Name", task.name());
	ASSERT_EQ("", task.UUID());
	ASSERT_EQ(false, task.completed());

}

TEST (Task, with)
{
	Task original("Apa", 0, "", false, 0, 0, "", false);
	auto now = time(0);

	auto task = original.withName("Test");
	ASSERT_EQ("Test", task.name());

	auto task2 = original.withParent(13);
	ASSERT_EQ(13, task2.parentID());

	auto task3 = original.withCompleted(true);
	ASSERT_EQ(true, task3.completed());

	auto task4 = original.withDeleted(true);
	ASSERT_EQ(true, task4.deleted());

	auto lastChanged = task4.lastChanged();
	ASSERT_TRUE(abs(lastChanged - now) < 2);

}

