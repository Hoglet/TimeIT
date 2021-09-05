#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/exceptions/db_exception.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST (TaskAccessor, getTask)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	task original("Test", {});
	taskAccessor.create(original);

	auto task1 = taskAccessor.by_id(original.id);
	ASSERT_EQ("Test", task1->name);
}

TEST (TaskAccessor, testGetTasks)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);

	task test_task("Test");
	taskAccessor.create(test_task);
	auto taskId = test_task.id;

	vector<task> tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(1, tasks.size());

	taskAccessor.create(task("NextTask", {}));
	taskAccessor.create(task("Sub task", taskId));
	tasks = taskAccessor.by_parent_id();
	ASSERT_EQ(2, tasks.size());

	taskAccessor.create(task("Test2"));

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
	task test_task("Test");
	taskAccessor.create(test_task);
	auto taskId = test_task.id;

	auto task1 = taskAccessor.by_id(taskId);
	ASSERT_EQ("Test", task1->name);
	taskAccessor.update(task1->with_name("Tjohopp"));
	auto task2 = taskAccessor.by_id(taskId);
	ASSERT_EQ("Tjohopp", task2->name);
}


TEST (TaskAccessor, remove)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	task test_task("Test");
	taskAccessor.create(test_task);
	auto taskId = test_task.id;
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
	}

	~TAObserver()
	{
	}

	void on_task_added( const task& /*t*/) override
	{
	}

	void on_task_updated( const task_id& id) override
	{
		updatedTaskID = id;
	}

	void on_task_removed( const task& item) override
	{
		removedTaskID = item.id;
	}

	void on_parent_changed( const task& item) override
	{
		updatedParentTaskID = item.id;
	}

	void on_task_name_changed(const task& item) override
	{
		nameChangedTaskID = item.id;
	}

	void on_task_time_changed(const task_id& id) override
	{
		timeChangedTaskID = id;
	}

	void on_complete_update() override
	{

	}

	optional<task_id> updatedTaskID = {};
	optional<task_id> updatedParentTaskID = {};
	optional<task_id> removedTaskID = {};
	optional<task_id> nameChangedTaskID = {};
	optional<task_id> timeChangedTaskID = {};


};

TEST (TaskAccessor, updateTask)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	TAObserver observer(notifier);
	task original_task("Test");
	task_accessor taskAccessor(tempdb);


	ASSERT_THROW(taskAccessor.update(original_task), db_exception) << "Should fail updating non existing task";


	taskAccessor.create(original_task);


	auto task1 = taskAccessor.by_id(original_task.id);

	observer.updatedTaskID = {};
	taskAccessor.update(task1->with_name("Coding"));
	auto changedTask = taskAccessor.by_id(original_task.id);
	ASSERT_EQ("Coding", changedTask->name);
	ASSERT_EQ( task1->id, observer.nameChangedTaskID.value()) << "Notified Task_ID: ";
	ASSERT_EQ( observer.updatedParentTaskID.has_value(), false) << "Notified ParentID: ";

	observer.nameChangedTaskID = {};

	task task2(*changedTask);
	taskAccessor.update(task2);
	auto changedTask2 = taskAccessor.by_id(original_task.id);
	ASSERT_EQ(observer.nameChangedTaskID.has_value(), false) << "Notified Task_ID: ";
	ASSERT_EQ( observer.updatedParentTaskID.has_value(), false) << "Notified ParentID: ";

	task subtask("Sub task");
	/*int64_t id2 = */taskAccessor.create(subtask);
	auto temp_task = taskAccessor.by_id(subtask.id);

	auto task3 = temp_task->with_parent(original_task.id);
	taskAccessor.update(task3);
	ASSERT_EQ( task3.id, observer.updatedParentTaskID.value()) << "Notified ParentID: ";

	taskAccessor.remove(task3.id);
	ASSERT_EQ( task3.id, observer.removedTaskID.value()) << "Notified ParentID: ";
}

TEST(TaskAccessor, lastChanged)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	task_accessor taskAccessor(tempdb);
	string originalName = "Test";
	task original_task(
			originalName,
			task_id(),
			system_clock::from_time_t(500),
			{},
			false,
			0min,
			false);

	taskAccessor.create(original_task);

	vector<task> tasks = taskAccessor.changed_since();
	ASSERT_EQ(1, tasks.size()) << "Asking for all tasks";
	task task1 = tasks.at(0);
	ASSERT_EQ(system_clock::from_time_t(500), task1.last_changed) << "Checking change time";
	tasks = taskAccessor.changed_since( system_clock::from_time_t(600) );
	ASSERT_EQ(0, tasks.size()) << "Asking for all tasks after last inserted";

	string newName = "New name";
	task updated_task(
			newName,
			task1.id,
			system_clock::from_time_t(495),
			task1.parent_id,
			false,
			0min,
			false);

	taskAccessor.update(updated_task);
	tasks = taskAccessor.changed_since();

	ASSERT_EQ(1, tasks.size()) <<
							   "Updated with task changed before task in database. Number of tasks should be unchanged";

	task task2 = tasks.at(0);

	ASSERT_EQ(originalName, task2.name) <<
										"Updated with task changed before task in database, name should not change";

}

}