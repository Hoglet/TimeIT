#include "parent_chooser.h"
#include <iostream>

using namespace libtimeit;
using namespace std;

namespace gui
{

using namespace std;
Parent_chooser::Parent_chooser(Database &database) :
		task_accessor(database)
{
	//Create the Tree model:
	//m_refTreeModel = Gtk::TreeStore::create(m_Columns);
	model = Gtk::ListStore::create(columns);
	set_model(model);

	//Fill the ComboBox's Tree Model:
	Gtk::TreeModel::Row row = *(model->append());
	row[columns.col_id] = 0;
	row[columns.col_name] = "-";

	string base_string;
	populate(base_string, 0);

	//Add the model columns to the Combo (which is a kind of view),
	//rendering them in the default way:
	//pack_start(m_Columns.m_col_id);
	pack_start(columns.col_name);

	set_active(0);
	//Connect signal handler:
	signal_changed().connect(sigc::mem_fun(*this, &Parent_chooser::on_combo_changed));

}

void Parent_chooser::set_id(Task_id id)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	iter = find_row(id);
	if (iter != children.end())
	{
		model->erase(iter);
	}
}

void Parent_chooser::set_parent(Task_id id)
{
	if (id >= 0)
	{
		this->parent_id = id;
	}
	else
	{
		this->parent_id = 0;
	}
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	iter = find_row(id);
	if (iter != children.end())
	{
		set_active(iter);
	}
}

Task_id Parent_chooser::get_parent_id() const
{
	return parent_id;
}

Gtk::TreeModel::iterator Parent_chooser::find_row(Task_id id)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void Parent_chooser::populate(std::string &base_string, Task_id parent_id_)
{
	vector<Task> tasks = task_accessor.by_parent_id(parent_id_);

	for (auto task: tasks)
	{
		Gtk::TreeModel::Row row;
		Gtk::TreeModel::iterator iter = model->append();
		row = *(iter);

		string name;
		if (base_string.length() > 0)
		{
			name = base_string;
		}
		name += task.name;
		row[columns.col_id] = task.id;
		row[columns.col_name] = name;

		string new_base_string = base_string + "    ";
		populate(new_base_string, task.id);
	}
}


void Parent_chooser::on_combo_changed()
{
	Gtk::TreeModel::iterator iter = get_active();
	if (iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row)
		{
			//Get the data for the selected row, using our knowledge of the tree
			//model:
			parent_id = row[columns.col_id];
		}
	}
	else
	{
		std::cout << "invalid iter" << std::endl;
	}
}

}
