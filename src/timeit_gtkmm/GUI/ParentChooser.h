/*
 * ParentChooser.h
 *
 *  Created on: Apr 26, 2009
 *      Author: hoglet
 */

#ifndef PARENTCHOOSER_H_
#define PARENTCHOOSER_H_

#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <libtimeit/db/Database.h>
#include <string>

namespace GUI
{

class ParentChooser: public Gtk::ComboBox
{
public:
	ParentChooser(std::shared_ptr<libtimeit::IDatabase>& database);
	virtual ~ParentChooser();

	void setID(int ID);
	void setParentID(int parentID);
	int getParentID();
private:
	//Signal handlers:
	virtual void on_combo_changed();
	void populate(std::string& baseString, int parentID);

	Gtk::TreeModel::iterator findRow(int id);

	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns()
		{
			add(col_id);
			add(col_name);
		}

		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
	};

	ModelColumns Columns;

	Glib::RefPtr<Gtk::ListStore> model;

	std::shared_ptr<libtimeit::ITaskAccessor> taskAccessor;
	int parentID;
};

}

#endif /* PARENTCHOOSER_H_ */
