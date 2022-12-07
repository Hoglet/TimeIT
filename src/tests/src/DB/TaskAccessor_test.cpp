#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/task_accessor.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST (TaskAccessor, getTask)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task original("Test", {});
	tasks.create( original);

	auto task_1 = tasks.by_id( original.id);
	ASSERT_EQ( "Test", task_1->name);
}

TEST (TaskAccessor, testGetTasks)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);

	task test_task("Test");
	tasks.create( test_task);
	auto task_id = test_task.id;

	vector<task> result = tasks.by_parent_id();
	ASSERT_EQ( 1, result.size());

	tasks.create( task( "NextTask", {}));
	tasks.create( task( "Sub task", task_id));
	result = tasks.by_parent_id();
	ASSERT_EQ( 2, result.size());

	tasks.create( task( "Test2"));

	result = tasks.by_parent_id();
	task &task_1 = result.at( 0);
	ASSERT_EQ( "Test", task_1.name);

	result = tasks.by_parent_id( task_id);
	ASSERT_EQ( 1, result.size()) << "Finding number of sub tasks ";

}

TEST (TaskAccessor, setTaskName)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task test_task("Test");
	tasks.create( test_task);
	auto task_id = test_task.id;

	auto task_1 = tasks.by_id( task_id);
	ASSERT_EQ( "Test", task_1->name);
	tasks.update( task_1->with_name( "Tjohopp"));
	auto task_2 = tasks.by_id( task_id);
	ASSERT_EQ( "Tjohopp", task_2->name);
}


TEST (TaskAccessor, remove)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	task test_task("Test");
	tasks.create( test_task);
	auto task_id = test_task.id;
	vector<task> result = tasks.by_parent_id();
	ASSERT_EQ( 1, result.size());
	tasks.remove( task_id);
	result = tasks.by_parent_id();
	ASSERT_EQ( 0, result.size());
}

TEST (TaskAccessor, newTask)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);

	task task_1( "Test");
	tasks.create( task_1);
	vector<task> result = tasks.by_parent_id();
	ASSERT_EQ( 1, result.size());
	task* taskp = &(result.at( 0));

	ASSERT_THROW( tasks.create( *taskp), db_exception) << "Adding an existing task should not be allowed";
}

class ta_observer : public event_observer
{
public:
	ta_observer( notification_manager &notifier) : event_observer( notifier)
	{
	}

	~ta_observer()
	{
	}

	void on_task_added( const task& /*t*/) override
	{
	}

	void on_task_updated( const task_id& id) override
	{
		updated_task_id = id;
	}

	void on_task_removed( const task& item) override
	{
		removed_task_id = item.id;
	}

	void on_parent_changed( const task& item) override
	{
		updated_parent_task_id = item.id;
	}

	void on_task_name_changed(const task& item) override
	{
		name_changed_task_id = item.id;
	}

	void on_task_time_changed(const task_id& id) override
	{
		time_changed_task_id = id;
	}

	void on_complete_update() override
	{

	}

	optional<task_id> updated_task_id = {};
	optional<task_id> updated_parent_task_id = {};
	optional<task_id> removed_task_id = {};
	optional<task_id> name_changed_task_id = {};
	optional<task_id> time_changed_task_id = {};


};

TEST (TaskAccessor, updateTask)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	ta_observer observer( notifier);
	task original_task("Test");
	task_accessor tasks( tempdb);


	ASSERT_THROW( tasks.update( original_task), db_exception) << "Should fail updating non existing task";


	tasks.create( original_task);


	auto task_1 = tasks.by_id( original_task.id);

	observer.updated_task_id = {};
	tasks.update( task_1->with_name( "Coding"));
	auto changed_task = tasks.by_id( original_task.id);
	ASSERT_EQ( "Coding", changed_task->name);
	ASSERT_EQ( task_1->id, observer.name_changed_task_id.value()) << "Notified Task_ID: ";
	ASSERT_EQ( observer.updated_parent_task_id.has_value(), false) << "Notified ParentID: ";

	observer.name_changed_task_id = {};

	task task_2( *changed_task);
	tasks.update( task_2);
	auto changed_task_2 = tasks.by_id( original_task.id);
	ASSERT_EQ( observer.name_changed_task_id.has_value(), false) << "Notified Task_ID: ";
	ASSERT_EQ( observer.updated_parent_task_id.has_value(), false) << "Notified ParentID: ";

	task subtask("Sub task");
	/*int64_t id2 = */tasks.create( subtask);
	auto temp_task = tasks.by_id( subtask.id);

	auto task_3 = temp_task->with_parent( original_task.id);
	tasks.update( task_3);
	ASSERT_EQ( task_3.id, observer.updated_parent_task_id.value()) << "Notified ParentID: ";

	tasks.remove( task_3.id);
	ASSERT_EQ( task_3.id, observer.removed_task_id.value()) << "Notified ParentID: ";
}

TEST(TaskAccessor, lastChanged)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	task_accessor tasks( tempdb);
	string original_name = "Test";
	task original_task(
			original_name,
			task_id(),
			system_clock::from_time_t(500),
			{},
			false,
			0min,
			false);

	tasks.create( original_task);

	vector<task> result = tasks.changed_since();
	ASSERT_EQ( 1, result.size()) << "Asking for all tasks";
	task task_1 = result.at( 0);
	ASSERT_EQ( system_clock::from_time_t(500), task_1.last_changed) << "Checking change time";
	result = tasks.changed_since( system_clock::from_time_t( 600) );
	ASSERT_EQ( 0, result.size()) << "Asking for all tasks after last inserted";

	string new_name = "New name";
	task updated_task(
			new_name,
			task_1.id,
			system_clock::from_time_t(495),
			task_1.parent_id,
			false,
			0min,
			false);

	tasks.update( updated_task);
	result = tasks.changed_since();

	ASSERT_EQ( 1, result.size()) <<
								 "Updated with task changed before task in database. Number of tasks should be unchanged";

	task task_2 = result.at( 0);

	ASSERT_EQ( original_name, task_2.name) <<
										   "Updated with task changed before task in database, name should not change";

}

}
