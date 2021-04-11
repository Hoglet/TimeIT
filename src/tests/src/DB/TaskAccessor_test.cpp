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
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	ASSERT_THROW(taskAccessor.create(Task("Test", 100)), db_exception);
}

TEST (TaskAccessor, getTask)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));

	auto task = taskAccessor.by_ID(taskId);
	ASSERT_EQ("Test", task->name );
}

TEST (TaskAccessor, testGetTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);

	int64_t taskId = taskAccessor.create(Task("Test", 0));

	vector<Task> tasks = taskAccessor.by_parent_ID();
	ASSERT_EQ(1, tasks.size());

	taskAccessor.create(Task("NextTask", 0));
	taskAccessor.create(Task("Sub task", taskId));
	tasks = taskAccessor.by_parent_ID();
	ASSERT_EQ(2, tasks.size());

	taskAccessor.create(Task("Test2", 0));

	tasks = taskAccessor.by_parent_ID();
	Task &task = tasks.at(0);
	ASSERT_EQ("Test", task.name);

	tasks = taskAccessor.by_parent_ID(taskId);
	ASSERT_EQ(1, tasks.size()) << "Finding number of sub tasks ";

}

TEST (TaskAccessor, setTaskName)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	auto task = taskAccessor.by_ID(taskId);
	ASSERT_EQ("Test", task->name);
	taskAccessor.update(task->with_name("Tjohopp"));
	auto task2 = taskAccessor.by_ID(taskId);
	ASSERT_EQ("Tjohopp", task2->name);
}

TEST (TaskAccessor, setParentID)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	int64_t taskId1 = taskAccessor.create(Task("Test", 0));
	int64_t taskId2 = taskAccessor.create(Task("Test2", 0));
	auto task = taskAccessor.by_ID(taskId1);
	ASSERT_EQ(0, task->parent_ID);
	taskAccessor.setParentID(taskId1, taskId2);
	auto task2 = taskAccessor.by_ID(taskId1);
	ASSERT_EQ(taskId2, task2->parent_ID);
}

TEST (TaskAccessor, setParentID_inputValidation)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	auto task = taskAccessor.by_ID(taskId);
	ASSERT_EQ(0, task->parent_ID);
	ASSERT_THROW(taskAccessor.setParentID(taskId, taskId + 1), db_exception);
}

TEST (TaskAccessor, remove)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.create(Task("Test", 0));
	vector<Task> tasks = taskAccessor.by_parent_ID();
	ASSERT_EQ(1, tasks.size());
	taskAccessor.remove(taskId);
	tasks = taskAccessor.by_parent_ID();
	ASSERT_EQ(0, tasks.size());
}

TEST (TaskAccessor, newTask)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);

	Task task("Test");
	taskAccessor.create(task);
	vector<Task> tasks = taskAccessor.by_parent_ID();
	ASSERT_EQ(1, tasks.size());
	Task *taskp = &(tasks.at(0));

	ASSERT_THROW(taskAccessor.create(*taskp), db_exception) << "Adding an existing task should not be allowed";
}

class TAObserver : public Event_observer
{
public:
	TAObserver(Notifier& notifier) : Event_observer(notifier)
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
	Notifier notifier;
	TempDB tempdb(notifier);
	TAObserver observer( notifier);
	Task original_task("Test");
	Task_accessor taskAccessor(tempdb);


	ASSERT_THROW(taskAccessor.update(original_task), db_exception) << "Should fail updating non existing task";

	int64_t id = taskAccessor.create(original_task);

	auto task1 = taskAccessor.by_ID(id);

	observer.updatedTaskID = 0;
	taskAccessor.update(task1->with_name("Coding"));
	auto changedTask = taskAccessor.by_ID(id);
	ASSERT_EQ("Coding", changedTask->name);
	ASSERT_EQ(task1->ID, observer.nameChangedTaskID) << "Notified Task_ID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	observer.nameChangedTaskID = 0;

	Task task2( *changedTask);
	taskAccessor.update(task2);
	auto changedTask2 = taskAccessor.by_ID(id);
	ASSERT_EQ(0, observer.nameChangedTaskID) << "Notified Task_ID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	Task subtask("Sub task");
	int64_t id2 = taskAccessor.create(subtask);
	auto temp_task = taskAccessor.by_ID(id2);

	auto task3 = temp_task->with_parent(id);
	taskAccessor.update(task3);
	ASSERT_EQ(task3.ID, observer.updatedParentTaskID) << "Notified ParentID: ";

	taskAccessor.remove(task3.ID);
	ASSERT_EQ(task3.ID, observer.removedTaskID) << "Notified ParentID: ";
}

TEST(TaskAccessor, lastChanged)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Task_accessor taskAccessor(tempdb);
	string originalName = "Test";
	Task original_task(originalName, 0, UUID(), false, 0, 500, {}, false);
	taskAccessor.create(original_task);

	vector<Task> tasks = taskAccessor.changed_since(0);
	ASSERT_EQ(1, tasks.size()) << "Asking for all tasks";
	Task task = tasks.at(0);
	ASSERT_EQ(500, task.last_changed) << "Checking change time";
	tasks = taskAccessor.changed_since(600);
	ASSERT_EQ(0, tasks.size()) << "Asking for all tasks after last inserted";

	string newName = "New name";
	Task updated_task(
			newName,
			task.parent_ID,
			task.uuid,
			task.completed,
			task.ID,
			495,
			task.parent_uuid,
			false);

	taskAccessor.update(updated_task);
	tasks = taskAccessor.changed_since(0);

	ASSERT_EQ(1, tasks.size()) <<
								"Updated with task changed before task in database. Number of tasks should be unchanged";

	Task task2 = tasks.at(0);

	ASSERT_EQ(originalName, task2.name) <<
										  "Updated with task changed before task in database, name should not change";

}


}