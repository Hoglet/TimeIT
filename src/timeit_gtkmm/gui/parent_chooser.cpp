#include "parent_chooser.h"
#include <iostream>

using namespace libtimeit;
using namespace std;

namespace gui
{

const task_id NO_PARENT( UUID::from_string("85d66103-41e6-4ea0-a204-f1d07990de70").value() );

using namespace std;
parent_chooser_widget::parent_chooser_widget(database &db) :
		tasks(db)
{
	//Create the Tree model:
	//m_refTreeModel = Gtk::TreeStore::create(m_Columns);
	model = Gtk::ListStore::create(columns);
	set_model(model);

	//Fill the ComboBox's Tree Model:
	Gtk::TreeModel::Row row = *(model->append());
	row[columns.col_id] = NO_PARENT;
	row[columns.col_name] = "-";

	string base_string;
	populate(base_string, {} );

	//Add the model columns to the Combo (which is a kind of view),
	//rendering them in the default way:
	//pack_start(m_Columns.m_col_id);
	pack_start(columns.col_name);

	set_active(0);
	//Connect signal handler:
	signal_changed().connect(sigc::mem_fun(*this, &parent_chooser_widget::on_combo_changed));

}

void parent_chooser_widget::set_id(const task_id& id)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	iter = find_row(id);
	if (iter != children.end())
	{
		model->erase(iter);
	}
}

void parent_chooser_widget::set_parent(optional<task_id> id)
{
	this->parent_id = id;

	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	if(id.has_value())
	{
		iter = find_row(id.value());
		if (iter != children.end())
		{
			set_active(iter);
		}
	}
}

optional<task_id> parent_chooser_widget::get_parent_id() const
{
	if ( parent_id == NO_PARENT)
	{
		return {};
	}
	return parent_id;
}

Gtk::TreeModel::iterator parent_chooser_widget::find_row(const task_id& id)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		Gtk::TreeModel::Row row = *iter;
		if ( row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void parent_chooser_widget::populate(std::string &base_string, optional<task_id> parent_id_)
{
	vector<task> child_tasks = tasks.by_parent_id(parent_id_);

	for (auto child: child_tasks)
	{
		Gtk::TreeModel::Row row;
		Gtk::TreeModel::iterator iter = model->append();
		row = *(iter);

		string name;
		if (base_string.length() > 0)
		{
			name = base_string;
		}
		name += child.name;
		row[columns.col_id] = child.id;
		row[columns.col_name] = name;

		string new_base_string = base_string + "    ";
		populate(new_base_string, child.id);
	}
}


void parent_chooser_widget::on_combo_changed()
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
