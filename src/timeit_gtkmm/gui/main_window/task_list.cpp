#include "task_list.h"
#include <glibmm/i18n.h>
#include <libtimeit/utils.h>

namespace gui
{
using namespace libtimeit;
using namespace std;
using namespace Gtk;
using namespace Glib;

task_list_widget::task_list_widget(
		database &db,
		time_manager &op_time_keeper,
		notification_manager &notifier,
		image_cache &images)
		:
		event_observer(notifier),
		tasks(db),
		time_keeper( op_time_keeper)
{
	// consider not loading and having these icons in memory multiple times across multiple classes
	running_icon = images.by_id(image_identifier::RUNNING);
	running_idle_icon = images.by_id(image_identifier::RUNNING_IDLE);
	blank_icon = images.by_id(image_identifier::BLANK);

	tree_model = TreeStore::create(columns);
	set_model(tree_model);
	append_column(_("Name"), columns.col_name);
	append_column("", columns.col_pixbuf);
	append_column(_("Time"), columns.col_time);
	Gtk::TreeView::Column* p_column = get_column(1);
	p_column->set_min_width(60); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	tree_model->set_sort_column(columns.col_name, Gtk::SORT_ASCENDING); // Initial sorting column
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &task_list_widget::on_selection_changed));
	populate();

	//Popup menu
	Gtk::Menu::MenuList &menu_list = menu_popup.items();

	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Start"), sigc::mem_fun(*this, &task_list_widget::on_menu_start)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Stop"), sigc::mem_fun(*this, &task_list_widget::on_menu_stop)));
	menu_list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Add time"), sigc::mem_fun(*this, &task_list_widget::on_menu_add_time)));
	menu_list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Add task"), sigc::mem_fun(*this, &task_list_widget::on_menu_add_task)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Edit task"), sigc::mem_fun(*this, &task_list_widget::on_menu_edit)));
	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Remove task"), sigc::mem_fun(*this, &task_list_widget::on_menu_remove_task)));

}

task_list_widget::~task_list_widget()
{
	std::list<action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		action_observer* observer = *iter;
		observer->on_action_task_selection_changed({});
	}
}

bool task_list_widget::on_button_press_event(GdkEventButton* event)
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

void task_list_widget::on_row_expanded(const TreeModel::iterator &iter, const TreeModel::Path & /*path*/)
{
	TreeModel::Row row = *iter;
	auto id = row[columns.col_id];
	tasks.set_task_expanded(id, true);
}

void task_list_widget::on_row_collapsed(const TreeModel::iterator &iter, const TreeModel::Path & /*path*/)
{
	TreeModel::Row row = *iter;
	auto id = row[columns.col_id];
	tasks.set_task_expanded(id, false);
}

void task_list_widget::on_task_added(const task& /*item*/)
{
	do_update();
}

void task_list_widget::on_task_updated(const task_id& id)
{
	auto updated_task = tasks.by_id(id);
	if (updated_task.has_value())
	{
		Gtk::TreeIter iter = find_row(id);
		if (iter != tree_model->children().end())
		{
			TreeModel::Row row = *iter;
			assign_values_to_row(row, *updated_task);
		}

		if (updated_task->parent_id.has_value())
		{
			on_task_updated(updated_task->parent_id.value());
		}
	}
}

void task_list_widget::on_task_name_changed(const task& item)
{
	on_task_updated(item.id);
}

void task_list_widget::on_task_time_changed(const task_id& id)
{
	on_task_updated(id);
}

void task_list_widget::on_task_removed(const task& item)
{
	Gtk::TreeIter iter = find_row(item.id);
	if (iter != tree_model->children().end())
	{
		tree_model->erase(iter);
	}
}

void task_list_widget::on_selection_changed()
{
	auto id = selected_id();
	for (auto* observer: observers)
	{
		observer->on_action_task_selection_changed(id);
	}
}

void task_list_widget::on_parent_changed(const task& /*item*/)
{
	do_update();
}

void task_list_widget::on_complete_update()
{
	do_update();
}

void task_list_widget::do_update()
{
	empty();
	populate();
}

void task_list_widget::empty()
{
	tree_model->clear();
}

optional<task_id> task_list_widget::selected_id()
{
	auto ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row = *iter;
		return row[columns.col_id];
		//Do something with the row.
	}
	return {};
}

Gtk::TreeModel::iterator task_list_widget::find_row(const task_id& id)
{
	TreeModel::Children children = tree_model->children();
	return sub_search(id, children);
}

Gtk::TreeModel::iterator task_list_widget::sub_search(const task_id& id, TreeModel::Children children)
{
	TreeIter iter;

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		TreeModel::Row row = *iter;
		if (!row.children().empty())
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

void task_list_widget::assign_values_to_row(TreeModel::Row &row, const extended_task &item)
{
	auto id = item.id;
	row[columns.col_id] = id;
	if (item.running)
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
	row[columns.col_name] = item.name;
	auto total_time = item.total_time;
	if (total_time > 0s)
	{
		row[columns.col_time] = libtimeit::seconds_2_hhmm(total_time);
	}
	else
	{
		row[columns.col_time] = "";
	}
}

void task_list_widget::populate(TreeModel::Row* parent, optional<task_id> parent_id)
{
	auto child_tasks = tasks.by_parent_id( parent_id);

	for (auto child_task: child_tasks)
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
		assign_values_to_row(row, child_task);
		populate(&row, child_task.id);
		if (child_task.expanded)
		{
			TreeModel::Path path(iter);
			this->expand_to_path(path);
		}
	}
}

void task_list_widget::on_menu_start()
{
	for (auto* observer: observers)
	{
		observer->on_action_start_task();
	}
}

void task_list_widget::on_menu_stop()
{
	for (auto* observer: observers)
	{
		observer->on_action_stop_task();
	}
}

void task_list_widget::on_menu_edit()
{
	for (auto* observer: observers)
	{
		observer->on_action_edit_task();
	}
}

void task_list_widget::on_menu_add_time()
{
	for (auto* observer: observers)
	{
		observer->on_action_add_time();
	}
}

void task_list_widget::on_menu_add_task()
{
	for (auto* observer: observers)
	{
		observer->on_action_add_task();
	}
}

void task_list_widget::on_menu_remove_task()
{
	for (auto* observer: observers)
	{
		observer->on_action_remove_task();
	}
}

void task_list_widget::attach(action_observer* observer)
{
	observers.push_back(observer);
}

void task_list_widget::detach(action_observer* observer)
{
	observers.remove(observer);
}
}
