#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "TempDB.h"
#include "TaskAccessor.h"
#include "dbexception.h"
#include "test_TaskAccessor.h"
#include <gtkmm.h>
#include <UUIDTool.h>

using namespace DB;

namespace Test
{
using namespace std;
void TaskAccessor_testNewTask_inputValidation()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	ASSERT_THROWS(taskAccessor->newTask("Test", 100), dbexception);
}

void TaskAccessor_getTask()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);

	shared_ptr<Task> task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Test", task->name());
}

void TaskAccessor_testGetTasks()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();

	int64_t taskId = taskAccessor->newTask("Test", 0);

	std::shared_ptr<std::vector<Task>> tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(1, tasks->size());

	taskAccessor->newTask("NextTask", 0);
	taskAccessor->newTask("Sub task", taskId);
	tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(2, tasks->size());

	taskAccessor->newTask("Test2", 0);

	tasks = taskAccessor->getTasks();
	Task &task = tasks->at(0);
	ASSERT_EQUAL("Test", task.name());

	tasks = taskAccessor->getTasks(taskId);
	ASSERT_EQUALM("Finding number of sub tasks ", 1, tasks->size());

}

void TaskAccessor_setTaskName()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	shared_ptr<Task> task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Test", task->name());
	taskAccessor->updateTask(task->withName("Tjohopp"));
	task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL("Tjohopp", task->name());
}

void TaskAccessor_setParentID()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId1 = taskAccessor->newTask("Test", 0);
	int64_t taskId2 = taskAccessor->newTask("Test2", 0);
	shared_ptr<Task> task = taskAccessor->getTask(taskId1);
	ASSERT_EQUAL(0, task->parentID());
	taskAccessor->setParentID(taskId1, taskId2);
	task = taskAccessor->getTask(taskId1);
	ASSERT_EQUAL(taskId2, task->parentID());
}

void TaskAccessor_setParentID_inputValidation()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	shared_ptr<Task> task = taskAccessor->getTask(taskId);
	ASSERT_EQUAL(0, task->parentID());
	ASSERT_THROWS(taskAccessor->setParentID(taskId, taskId + 1), dbexception);
}

void TaskAccessor_remove()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	int64_t taskId = taskAccessor->newTask("Test", 0);
	std::shared_ptr<std::vector<Task>> tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(1, tasks->size());
	taskAccessor->removeTask(taskId);
	tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(0, tasks->size());
}

void TaskAccessor_newTask()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();

	Task task("Test");
	taskAccessor->newTask(task);
	std::shared_ptr<std::vector<Task>> tasks = taskAccessor->getTasks();
	ASSERT_EQUAL(1, tasks->size());
	Task *taskp = &(tasks->at(0));
	ASSERTM("UUID should be set", taskp->UUID().length() != 0);
	ASSERTM("UUID should be valid", UUIDTool::isValid(taskp->UUID()));

	ASSERT_THROWSM("Adding an existing task should not be allowed", taskAccessor->newTask(*taskp), dbexception);
}

class TAObserver: public TaskAccessorObserver
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

void TaskAccessor_updateTask()
{
	TempDB tempdb;
	TAObserver observer;
	Task original_task("Test");
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	taskAccessor->attach(&observer);

	ASSERT_THROWSM("Should fail updating non existing task", taskAccessor->updateTask(original_task), dbexception);

	int64_t id = taskAccessor->newTask(original_task);

	shared_ptr<Task> task1 = taskAccessor->getTask(id);

	observer.updatedTaskID = 0;
	taskAccessor->updateTask(task1->withName("Coding"));
	shared_ptr<Task> changedTask = taskAccessor->getTask(id);
	Gtk::Main::iteration(false);
	ASSERT_EQUAL("Coding", changedTask->name());
	ASSERT_EQUALM("Notified TaskID: ", task1->ID(), observer.nameChangedTaskID);
	ASSERT_EQUALM("Notified ParentID: ", 0, observer.updatedParentTaskID);

	observer.nameChangedTaskID = 0;

	task1 = changedTask;
	taskAccessor->updateTask(*task1);
	changedTask = taskAccessor->getTask(id);
	ASSERT_EQUALM("Notified TaskID: ", 0, observer.nameChangedTaskID);
	ASSERT_EQUALM("Notified ParentID: ", 0, observer.updatedParentTaskID);

	Task subtask("Sub task");
	int64_t id2 = taskAccessor->newTask(subtask);
	shared_ptr<Task> temp_task = taskAccessor->getTask(id2);

	auto task2 = temp_task->withParent(id);
	taskAccessor->updateTask(task2);
	Gtk::Main::iteration(false);
	ASSERT_EQUALM("Notified ParentID: ", task2.ID(), observer.updatedParentTaskID);

	taskAccessor->removeTask(task2.ID());
	Gtk::Main::iteration(false);
	ASSERT_EQUALM("Notified ParentID: ", task2.ID(), observer.removedTaskID);
}

void TaskAccessor_lastChanged()
{
	TempDB tempdb;
	std::shared_ptr<ITaskAccessor> taskAccessor = tempdb.getTaskAccessor();
	std::string originalName = "Test";
	Task original_task(originalName, 0, UUIDTool::randomUUID(), false, 0, 500, "", false);
	taskAccessor->newTask(original_task);

	std::shared_ptr<std::vector<Task> > tasks = taskAccessor->getTasksChangedSince(0);
	ASSERT_EQUALM("Asking for all tasks", 1, tasks->size());
	Task task = tasks->at(0);
	ASSERT_EQUALM("Checking change time", 500, task.lastChanged());
	tasks = taskAccessor->getTasksChangedSince(600);
	ASSERT_EQUALM("Asking for all tasks after last inserted", 0, tasks->size());

	std::string newName = "New name";
	Task updated_task(newName, task.parentID(), task.UUID(), task.completed(), task.ID(), 495, task.parentUUID(), false);
	taskAccessor->updateTask(updated_task);
	tasks = taskAccessor->getTasksChangedSince(0);

	ASSERT_EQUALM("Updated with task changed before task in database. Number of tasks should be unchanged", 1, tasks->size());
	Task task2 = tasks->at(0);
	ASSERT_EQUALM("Updated with task changed before task in database, name should not change", originalName, task2.name());

}

cute::suite make_suite_test_TaskAccessor()
{
	cute::suite s;
	s.push_back(CUTE(TaskAccessor_getTask));
	s.push_back(CUTE(TaskAccessor_testGetTasks));
	s.push_back(CUTE(TaskAccessor_setTaskName));
	s.push_back(CUTE(TaskAccessor_setParentID));
	s.push_back(CUTE(TaskAccessor_setParentID_inputValidation));
	s.push_back(CUTE(TaskAccessor_remove));
	s.push_back(CUTE(TaskAccessor_testNewTask_inputValidation));
	s.push_back(CUTE(TaskAccessor_newTask));
	s.push_back(CUTE(TaskAccessor_updateTask));
	s.push_back(CUTE(TaskAccessor_lastChanged));
	return s;
}

} /* namespace Test */
