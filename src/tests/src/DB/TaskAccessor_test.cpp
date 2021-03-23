#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/TaskAccessor.h>
#include <libtimeit/exceptions/dbexception.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST (TaskAccessor, NewTask_inputValidation)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	ASSERT_THROW(taskAccessor.newTask("Test", 100), dbexception);
}

TEST (TaskAccessor, getTask)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.newTask("Test", 0);

	auto task = taskAccessor.getTask(taskId);
	ASSERT_EQ("Test", task->name()
	);
}

TEST (TaskAccessor, testGetTasks)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);

	int64_t taskId = taskAccessor.newTask("Test", 0);

	vector<Task> tasks = taskAccessor.getTasks();
	ASSERT_EQ(1, tasks.size());

	taskAccessor.newTask("NextTask", 0);
	taskAccessor.newTask("Sub task", taskId);
	tasks = taskAccessor.getTasks();
	ASSERT_EQ(2, tasks.size());

	taskAccessor.newTask("Test2", 0);

	tasks = taskAccessor.getTasks();
	Task &task = tasks.at(0);
	ASSERT_EQ("Test", task.name());

	tasks = taskAccessor.getTasks(taskId);
	ASSERT_EQ(1, tasks.size()) << "Finding number of sub tasks ";

}

TEST (TaskAccessor, setTaskName)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.newTask("Test", 0);
	auto task = taskAccessor.getTask(taskId);
	ASSERT_EQ("Test", task->name());
	taskAccessor.updateTask(task->withName("Tjohopp"));
	auto task2 = taskAccessor.getTask(taskId);
	ASSERT_EQ("Tjohopp", task2->name());
}

TEST (TaskAccessor, setParentID)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	int64_t taskId1 = taskAccessor.newTask("Test", 0);
	int64_t taskId2 = taskAccessor.newTask("Test2", 0);
	auto task = taskAccessor.getTask(taskId1);
	ASSERT_EQ(0, task->parentID());
	taskAccessor.setParentID(taskId1, taskId2);
	auto task2 = taskAccessor.getTask(taskId1);
	ASSERT_EQ(taskId2, task2->parentID());
}

TEST (TaskAccessor, setParentID_inputValidation)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.newTask("Test", 0);
	auto task = taskAccessor.getTask(taskId);
	ASSERT_EQ(0, task->parentID());
	ASSERT_THROW(taskAccessor.setParentID(taskId, taskId + 1), dbexception);
}

TEST (TaskAccessor, remove)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	int64_t taskId = taskAccessor.newTask("Test", 0);
	vector<Task> tasks = taskAccessor.getTasks();
	ASSERT_EQ(1, tasks.size());
	taskAccessor.removeTask(taskId);
	tasks = taskAccessor.getTasks();
	ASSERT_EQ(0, tasks.size());
}

TEST (TaskAccessor, newTask)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);

	Task task("Test");
	taskAccessor.newTask(task);
	vector<Task> tasks = taskAccessor.getTasks();
	ASSERT_EQ(1, tasks.size());
	Task *taskp = &(tasks.at(0));

	ASSERT_THROW(taskAccessor.newTask(*taskp), dbexception) << "Adding an existing task should not be allowed";
}

class TAObserver : public EventObserver
{
public:
	TAObserver()
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

	virtual void on_taskAdded(int64_t)
	{
	}

	virtual void on_taskUpdated(int64_t id)
	{
		updatedTaskID = id;
	}

	virtual void on_taskRemoved(int64_t id)
	{
		removedTaskID = id;
	}

	virtual void on_taskParentChanged(int64_t id)
	{
		updatedParentTaskID = id;
	}

	virtual void on_taskNameChanged(int64_t id)
	{
		nameChangedTaskID = id;
	}

	virtual void on_taskTimeChanged(int64_t id)
	{
		timeChangedTaskID = id;
	}

	virtual void on_completeUpdate()
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
	TAObserver observer;
	Task original_task("Test");
	TaskAccessor taskAccessor(tempdb);
	taskAccessor.attach(&observer);

	ASSERT_THROW(taskAccessor.updateTask(original_task), dbexception) << "Should fail updating non existing task";

	int64_t id = taskAccessor.newTask(original_task);

	auto task1 = taskAccessor.getTask(id);

	observer.updatedTaskID = 0;
	taskAccessor.updateTask(task1->withName("Coding"));
	auto changedTask = taskAccessor.getTask(id);
	ASSERT_EQ("Coding", changedTask->name());
	ASSERT_EQ(task1->ID(), observer.nameChangedTaskID) << "Notified TaskID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	observer.nameChangedTaskID = 0;

	Task task2( *changedTask);
	taskAccessor.updateTask(task2);
	auto changedTask2 = taskAccessor.getTask(id);
	ASSERT_EQ(0, observer.nameChangedTaskID) << "Notified TaskID: ";
	ASSERT_EQ(0, observer.updatedParentTaskID) << "Notified ParentID: ";

	Task subtask("Sub task");
	int64_t id2 = taskAccessor.newTask(subtask);
	auto temp_task = taskAccessor.getTask(id2);

	auto task3 = temp_task->withParent(id);
	taskAccessor.updateTask(task3);
	ASSERT_EQ(task3.ID(), observer.updatedParentTaskID) << "Notified ParentID: ";

	taskAccessor.removeTask(task3.ID());
	ASSERT_EQ(task3.ID(), observer.removedTaskID) << "Notified ParentID: ";
}

TEST(TaskAccessor, lastChanged)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	TaskAccessor taskAccessor(tempdb);
	string originalName = "Test";
	Task original_task(originalName, 0, UUID(), false, 0, 500, {}, false);
	taskAccessor.newTask(original_task);

	vector<Task> tasks = taskAccessor.getTasksChangedSince(0);
	ASSERT_EQ(1, tasks.size()) << "Asking for all tasks";
	Task task = tasks.at(0);
	ASSERT_EQ(500, task.lastChanged()) << "Checking change time";
	tasks = taskAccessor.getTasksChangedSince(600);
	ASSERT_EQ(0, tasks.size()) << "Asking for all tasks after last inserted";

	string newName = "New name";
	Task updated_task(
			newName,
			task.parentID(),
			task.getUUID(),
			task.completed(),
			task.ID(),
			495,
			task.parentUUID(),
			false);

	taskAccessor.updateTask(updated_task);
	tasks = taskAccessor.getTasksChangedSince(0);

	ASSERT_EQ(1, tasks.size()) <<
								"Updated with task changed before task in database. Number of tasks should be unchanged";

	Task task2 = tasks.at(0);

	ASSERT_EQ(originalName, task2.name()) <<
										  "Updated with task changed before task in database, name should not change";

}


}