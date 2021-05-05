/*
 * ParentChooser.h
 *
 *  Created on: Apr 26, 2009
 *      Author: hoglet
 */

#ifndef PARENTCHOOSER_H_
#define PARENTCHOOSER_H_

#include <string>
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>

namespace gui
{
using namespace libtimeit;
using namespace std;

class ParentChooser: public Gtk::ComboBox
{
public:
	ParentChooser(Database& database);
	virtual ~ParentChooser() = default;

	void setID(Task_id ID);
	void setParentID(Task_id parentID);
	int getParentID();
private:
	//Signal handlers:
	virtual void on_combo_changed();
	void populate(string& baseString, int parentID);

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

	Task_accessor taskAccessor;
	int parentID;
};

}

#endif /* PARENTCHOOSER_H_ */
