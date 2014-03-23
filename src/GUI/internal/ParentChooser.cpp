/*
 * ParentChooser.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: hoglet
 */

#include "ParentChooser.h"
#include <iostream>
#include <ExtendedTask.h>

using namespace DB;

namespace GUI
{
namespace Internal
{
using namespace std;
ParentChooser::ParentChooser(std::shared_ptr<DB::IDatabase>& database)
{
	taskAccessor = database->getTaskAccessor();
	//Create the Tree model:
	//m_refTreeModel = Gtk::TreeStore::create(m_Columns);
	model = Gtk::ListStore::create(Columns);
	set_model(model);

	//Fill the ComboBox's Tree Model:
	Gtk::TreeModel::Row row = *(model->append());
	row[Columns.col_id] = 0;
	row[Columns.col_name] = "-";

	std::string baseString = "";
	populate(baseString, 0);
	parentID = 0;

	//Add the model columns to the Combo (which is a kind of view),
	//rendering them in the default way:
	//pack_start(m_Columns.m_col_id);
	pack_start(Columns.col_name);

	set_active(0);
	//Connect signal handler:
	signal_changed().connect(sigc::mem_fun(*this, &ParentChooser::on_combo_changed));

}

void ParentChooser::setID(int ID)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	iter = findRow(ID);
	if (iter != children.end())
	{
		model->erase(iter);
	}
}

void ParentChooser::setParentID(int parentID)
{
	if(parentID>=0)
	{
		this->parentID = parentID;
	}
	else
	{
		this->parentID = 0;
	}
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();
	iter = findRow(parentID);
	if (iter != children.end())
	{
		set_active(iter);
	}
}


int ParentChooser::getParentID()
{
	return parentID;
}

Gtk::TreeModel::iterator ParentChooser::findRow(int id)
{
	Gtk::TreeIter iter;
	Gtk::TreeModel::Children children = model->children();

	for (iter = children.begin(); iter != children.end(); iter++)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row[Columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void ParentChooser::populate(std::string& baseString, int parentID)
{
	std::shared_ptr<vector<Task>> tasks = taskAccessor->getTasks(parentID);

	for (int i = 0; i < (int) tasks->size(); i++)
	{
		Gtk::TreeModel::Row row;
		Gtk::TreeModel::iterator iter = model->append();
		row = *(iter);
		Task& task = tasks->at(i);

		string name;
		if (baseString.length() > 0)
		{
			name = baseString;
		}
		name += task.getName();
		row[Columns.col_id] = task.getID();
		row[Columns.col_name] = name;

		string newBaseString = baseString + "    ";
		populate(newBaseString, task.getID());
	}
}

ParentChooser::~ParentChooser()
{
}

void ParentChooser::on_combo_changed()
{
	Gtk::TreeModel::iterator iter = get_active();
	if (iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if (row)
		{
			//Get the data for the selected row, using our knowledge of the tree
			//model:
			parentID = row[Columns.col_id];
//			Glib::ustring name = row[Columns.col_name];
//			std::cout << " ID=" << id << ", name=" << name << std::endl;
		}
	}
	else
		std::cout << "invalid iter" << std::endl;
}

}
}
