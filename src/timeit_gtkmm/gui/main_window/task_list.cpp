#include "task_list.h"
#include <string>
#include <glibmm/i18n.h>
#include <libtimeit/utils.h>

namespace gui
{
using namespace libtimeit;
using namespace std;
using namespace Gtk;
using namespace Glib;
Task_list::Task_list(
		Database&    database,
		Time_keeper& timeKeeper,
		Notifier&    notifier,
		Images&      images)
		:
		Event_observer(notifier),
		task_accessor(database),
		time_keeper(timeKeeper)

{
	// consider not loading and having these icons in memory multiple times across multiple classes
	running_icon      = images.by_id(image_identifier::RUNNING);
	running_idle_icon = images.by_id(image_identifier::RUNNING_IDLE);
	blank_icon        = images.by_id(image_identifier::BLANK);

	tree_model = TreeStore::create(columns);
	set_model(tree_model);
	append_column(_("Name"), columns.col_name);
	append_column("", columns.col_pixbuf);
	append_column(_("Time"), columns.col_time);
	Gtk::TreeView::Column *p_column = get_column(1);
	p_column->set_min_width(60); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	tree_model->set_sort_column(columns.col_name, Gtk::SORT_ASCENDING); // Initial sorting column
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Task_list::on_selection_changed));
	populate();

	//Popup menu
	Gtk::Menu::MenuList &menu_list = menu_popup.items();

	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Start"), sigc::mem_fun(*this, &Task_list::on_menu_start)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(*this, &Task_list::on_menu_stop)));
	menu_list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(*this, &Task_list::on_menu_add_time)));
	menu_list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(*this, &Task_list::on_menu_add_task)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Edit task"), sigc::mem_fun(*this, &Task_list::on_menu_edit)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(*this, &Task_list::on_menu_remove_task)));

}

Task_list::~Task_list()
{
	Task_id selected_id = 0;
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer *observer = *iter;
		observer->on_action_task_selection_changed(selected_id);
	}
}

bool Task_list::on_button_press_event(GdkEventButton *event)
{
	bool retval = TreeView::on_button_press_event(event);
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		menu_popup.popup(event->button, event->time);
		retval = true; //It has been handled.
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		on_menu_add_time();
		retval = true;
	}
	return retval;
}

void Task_list::on_row_expanded(const TreeModel::iterator &iter, const TreeModel::Path& /*path*/)
{
	TreeModel::Row row = *iter;
	auto id = row[columns.col_id];
	task_accessor.set_task_expanded(id, true);
}
void Task_list::on_row_collapsed(const TreeModel::iterator &iter, const TreeModel::Path& /*path*/)
{
	TreeModel::Row row = *iter;
	auto id = row[columns.col_id];
	task_accessor.set_task_expanded(id, false);
}

void Task_list::on_task_added(int64_t /*id*/)
{
	do_update();
}

void Task_list::on_task_updated(int64_t taskID)
{
	auto task = task_accessor.by_id(taskID);
	if (task.has_value())
	{
		Gtk::TreeIter iter = find_row(taskID);
		if (iter != tree_model->children().end())
		{
			TreeModel::Row row = *iter;
			assign_values_to_row(row, *task);
		}

		if (task->parent_id > 0)
		{
			on_task_updated(task->parent_id);
		}
	}
}

void Task_list::on_task_name_changed(int64_t taskID)
{
	on_task_updated(taskID);
}

void Task_list::on_task_time_changed(int64_t taskID)
{
	on_task_updated(taskID);
}

void Task_list::on_task_removed(int64_t taskID)
{
	Gtk::TreeIter iter = find_row(taskID);
	if (iter != tree_model->children().end())
	{
		tree_model->erase(iter);
	}
}

void Task_list::on_selection_changed()
{
	auto id = selected_id();
	for (auto* observer: observers)
	{
		observer->on_action_task_selection_changed(id);
	}
}

void Task_list::on_parent_changed(int64_t /*id*/)
{
	do_update();
}

void Task_list::on_complete_update()
{
	do_update();
}

void Task_list::do_update()
{
	empty();
	populate();
}

void Task_list::empty()
{
	tree_model->clear();
}
Task_id Task_list::selected_id()
{
	Task_id return_value = 0;
	auto ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row = *iter;
		return_value = row[columns.col_id];
		//Do something with the row.
	}
	return return_value;
}

Gtk::TreeModel::iterator Task_list::find_row(Task_id id)
{
	TreeModel::Children children = tree_model->children();
	return sub_search(id, children);
}

Gtk::TreeModel::iterator Task_list::sub_search(Task_id id, TreeModel::Children children)
{
	TreeIter iter;

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		TreeModel::Row row = *iter;
		if ( !row.children().empty() )
		{
			TreeIter sub_iterator = sub_search(id, row.children());
			if (sub_iterator != row.children().end())
			{
				iter = sub_iterator;
				break;
			}
		}
		if (row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void Task_list::assign_values_to_row(TreeModel::Row &row, const Extended_task &task)
{
	Task_id task_id = task.id;
	row[columns.col_id] = task_id;
	if (task.running)
	{
		if (!time_keeper.tasks_are_running())
		{
			row[columns.col_pixbuf] = running_icon;
		}
		else
		{
			row[columns.col_pixbuf] = running_idle_icon;
		}
	}
	else
	{
		row[columns.col_pixbuf] = blank_icon;
	}
	row[columns.col_name] = task.name;
	time_t total_time = task.total_time;
	if (total_time > 0)
	{
		row[columns.col_time] = libtimeit::seconds_2_hhmm(total_time);
	}
	else
	{
		row[columns.col_time] = "";
	}
}

void Task_list::populate(TreeModel::Row *parent, Task_id parentID)
{
	auto tasks = task_accessor.by_parent_id(parentID);

	for (auto task: tasks)
	{
		TreeModel::Row row;
		TreeModel::iterator iter;
		if (parent != nullptr)
		{
			iter = tree_model->append(parent->children());
		}
		else
		{
			iter = tree_model->append();
		}
		row = *iter;
		assign_values_to_row(row, task);
		populate(&row, task.id);
		if (task.expanded)
		{
			TreeModel::Path path(iter);
			this->expand_to_path(path);
		}
	}
}

void Task_list::on_menu_start()
{
	for (auto* observer: observers)
	{
		observer->on_action_start_task();
	}
}
void Task_list::on_menu_stop()
{
	for (auto* observer: observers)
	{
		observer->on_action_stop_task();
	}
}
void Task_list::on_menu_edit()
{
	for (auto* observer: observers)
	{
		observer->on_action_edit_task();
	}
}
void Task_list::on_menu_add_time()
{
	for (auto* observer: observers)
	{
		observer->on_action_add_time();
	}
}
void Task_list::on_menu_add_task()
{
	for (auto* observer: observers)
	{
		observer->on_action_add_task();
	}
}

void Task_list::on_menu_remove_task()
{
	for (auto* observer: observers)
	{
		observer->on_action_remove_task();
	}
}

void Task_list::attach(Action_observer *observer)
{
	observers.push_back(observer);
}
void Task_list::detach(Action_observer *observer)
{
	observers.remove(observer);
}
}
