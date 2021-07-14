#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/exceptions/db_exception.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST (TaskAccessor, NewTask_inputValidation)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	ASSERT_THROW(task_accessor(tempdb).create(task("Test", 100)), db_exception);
}

TEST (TaskAccessor, getTask)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(task("Test", 0));

	auto task1 = taskAccessor.by_id(taskId);
	ASSERT_EQ("Test", task1->name);
}

TEST (TaskAccessor, testGetTasks)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);

	int64_t taskId = taskAccessor.create(task("Test", 0));

	vector<task> tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(1, tasks.size());

	taskAccessor.create(task("NextTask", 0));
	taskAccessor.create(task("Sub task", taskId));
	tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(2, tasks.size());

	taskAccessor.create(task("Test2", 0));

	tasks = taskAccessor.by_parent_id();
	task &task1 = tasks.at(0);
	ASSERT_EQ("Test", task1.name);

	tasks = taskAccessor.by_parent_id(taskId);
	ASSERT_EQ(1, tasks.size()) << "Finding number of sub tasks ";

}

TEST (TaskAccessor, setTaskName)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(task("Test", 0));
	auto task1 = taskAccessor.by_id(taskId);
	ASSERT_EQ("Test", task1->name);
	taskAccessor.update(task1->with_name("Tjohopp"));
	auto task2 = taskAccessor.by_id(taskId);
	ASSERT_EQ("Tjohopp", task2->name);
}

TEST (TaskAccessor, setParentID)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	int64_t taskId1 = taskAccessor.create(task("Test", 0));
	int64_t taskId2 = taskAccessor.create(task("Test2", 0));
	auto task1 = taskAccessor.by_id(taskId1);
	ASSERT_EQ(0, task1->parent_id);
	taskAccessor.set_parent_id(taskId1, taskId2);
	auto task2 = taskAccessor.by_id(taskId1);
	ASSERT_EQ(taskId2, task2->parent_id);
}

TEST (TaskAccessor, setParentID_inputValidation)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(task("Test", 0));
	auto task1 = taskAccessor.by_id(taskId);
	ASSERT_EQ(0, task1->parent_id);
	ASSERT_THROW(taskAccessor.set_parent_id(taskId, taskId + 1), db_exception);
}

TEST (TaskAccessor, remove)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(task("Test", 0));
	vector<task> tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(1, tasks.size());
	taskAccessor.remove(taskId);
	tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(0, tasks.size());
}

TEST (TaskAccessor, newTask)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);

	task task1("Test");
	taskAccessor.create(task1);
	vector<task> tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(1, tasks.size());
	task* taskp = &(tasks.at(0));

	ASSERT_THROW(taskAccessor.create(*taskp), db_exception) << "Adding an existing task should not be allowed";
}

class TAObserver : public event_observer
{
public:
	TAObserver(notification_manager &notifier) : event_observer(notifier)
	{
		updatedTaskID = 0;
		updatedParentTaskID = 0;
		removedTaskID = 0;
		nameChangedTaskID = 0;
		timeChangedTaskID = 0;
	}

	~TAObserver()
	{
	}

	virtual void on_task_added(int64_t)
	{
	}

	virtual void on_task_updated(int64_t id)
	{
		updatedTaskID = id;
	}

	virtual void on_task_removed(int64_t id)
	{
		removedTaskID = id;
	}

	virtual void on_parent_changed(int64_t id)
	{
		updatedParentTaskID = id;
	}

	virtual void on_task_name_changed(int64_t id)
	{
		nameChangedTaskID = id;
	}

	virtual void on_task_time_changed(int64_t id)
	{
		timeChangedTaskID = id;
	}

	virtual void on_complete_update()
	{

	}

	int64_t updatedTaskID;
	int64_t updatedParentTaskID;
	int64_t removedTaskID;
	int64_t nameChangedTaskID;
	int64_t timeChangedTaskID;


};

TEST (TaskAccessor, updateTask)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	TAObserver observer(notifier);
	task original_task("Test");
	task_accessor taskAccessor(tempdb);


	ASSERT_THROW(taskAccessor.update(original_task), db_exception) << "Should fail updating non existing task";

	int64_t id = taskAccessor.create(original_task);

	auto task1 = taskAccessor.by_id(id);

	observer.updatedTaskID = 0;
	taskAccessor.update(task1->with_name("Coding"));
	auto changedTask = taskAccessor.by_id(id);
	ASSERT_EQ("Coding", changedTask->name);
	ASSERT_EQ(task1->id, observer.nameChangedTaskID) << "Notified Task_ID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	observer.nameChangedTaskID = 0;

	task task2(*changedTask);
	taskAccessor.update(task2);
	auto changedTask2 = taskAccessor.by_id(id);
	ASSERT_EQ(0, observer.nameChangedTaskID) << "Notified Task_ID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	task subtask("Sub task");
	int64_t id2 = taskAccessor.create(subtask);
	auto temp_task = taskAccessor.by_id(id2);

	auto task3 = temp_task->with_parent(id);
	taskAccessor.update(task3);
	ASSERT_EQ(task3.id, observer.updatedParentTaskID) << "Notified ParentID: ";

	taskAccessor.remove(task3.id);
	ASSERT_EQ(task3.id, observer.removedTaskID) << "Notified ParentID: ";
}

TEST(TaskAccessor, lastChanged)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	string originalName = "Test";
	task original_task(originalName, 0, UUID(), false, 0, 500, {}, false, 0, false);
	taskAccessor.create(original_task);

	vector<task> tasks = taskAccessor.changed_since(0);
	ASSERT_EQ(1, tasks.size()) << "Asking for all tasks";
	task task1 = tasks.at(0);
	ASSERT_EQ(500, task1.last_changed) << "Checking change time";
	tasks = taskAccessor.changed_since(600);
	ASSERT_EQ(0, tasks.size()) << "Asking for all tasks after last inserted";

	string newName = "New name";
	task updated_task(
			newName,
			task1.parent_id,
			task1.uuid,
			task1.completed,
			task1.id,
			495,
			task1.parent_uuid,
			false,
			0,
			false);

	taskAccessor.update(updated_task);
	tasks = taskAccessor.changed_since(0);

	ASSERT_EQ(1, tasks.size()) <<
							   "Updated with task changed before task in database. Number of tasks should be unchanged";

	task task2 = tasks.at(0);

	ASSERT_EQ(originalName, task2.name) <<
										"Updated with task changed before task in database, name should not change";

}

TEST(TaskAccessor, id)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	string originalName = "Test";
	task task1(originalName, 0, UUID(), false, 0, 500, {}, false, 0, false);
	auto expected_id = taskAccessor.create(task1);
	auto id = taskAccessor.id(task1.uuid);
	ASSERT_EQ(expected_id, id);
}

}