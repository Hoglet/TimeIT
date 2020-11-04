#include "test_task.h"
#include "Task.h"
#include <cute.h>
#include <cute_runner.h>
#include <exception>
#include <stdexcept>

using namespace DB;

namespace Test
{
class TestTaskC
{
public:
	TestTaskC()
	{
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

void Task_Creation()
{
	TestTaskC test_object;
	ASSERT_THROWSM("Invalid uuid not allowed", test_object.testInvalidUUID(), std::invalid_argument);
	ASSERT_THROWSM("Invalid parent uuid not allowed", test_object.testInvalidParentUUID(), std::invalid_argument);
}

void Task_settersAndGetters()
{
	Task task("Name");
	ASSERT_EQUALM("Id should be 0", 0, task.getID());
	ASSERT_EQUALM("Parent should be 0", 0, task.getParentID());

	ASSERT_EQUALM("Name should be \"Name\"", "Name", task.getName());
	ASSERT_EQUALM("UUID should be \"\"", "", task.getUUID());
	ASSERT_EQUALM("Completed should be false", false, task.getCompleted());

}

void Task_with()
{
	Task original("Apa", 0, "", false, 0, 0, "", false);
	auto now = time(0);

	Task task = original.withName("Test");
	ASSERT_EQUALM("Name should be \"Test\"", "Test", task.getName());

	task = original.withParent(13);
	ASSERT_EQUALM("Id should be 13", 13, task.getParentID());

	task = original.withCompleted(true);
	ASSERT_EQUALM("Completed should be true", true, task.getCompleted());

	task = original.withDeleted(true);
	ASSERT_EQUALM("Deleted should be true", true, task.getDeleted());

	auto lastChanged = task.getLastChanged();
	ASSERT_EQUAL_DELTAM("Last changed is wrong", 0, abs(lastChanged - now), 2);

}

cute::suite make_suite_Task_test()
{
	cute::suite s;
	s.push_back(CUTE(Task_Creation));
	s.push_back(CUTE(Task_settersAndGetters));
	s.push_back(CUTE(Task_with));
	return s;
}

} /* namespace Test */
