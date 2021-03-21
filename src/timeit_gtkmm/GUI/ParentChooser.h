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
using namespace libtimeit;
using namespace std;

class ParentChooser: public Gtk::ComboBox
{
public:
	ParentChooser(IDatabase& database);
	virtual ~ParentChooser();

	void setID(int ID);
	void setParentID(int parentID);
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

	shared_ptr<ITaskAccessor> taskAccessor;
	int parentID;
};

}

#endif /* PARENTCHOOSER_H_ */
