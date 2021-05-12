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

class Parent_chooser: public Gtk::ComboBox
{
public:
	Parent_chooser(Database& database);

	void    set_id(Task_id id);
	void    set_parent(Task_id id);
	Task_id get_parent_id() const;
private:
	//Signal handlers:
	virtual void on_combo_changed();
	void populate(string& base_string, Task_id parent_id);

	Gtk::TreeModel::iterator find_row(Task_id id);

	//Tree model columns:
	class Model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		Model_columns()
		{
			add(col_id);
			add(col_name);
		}

		Gtk::TreeModelColumn<Task_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
	};

	Model_columns columns;

	Glib::RefPtr<Gtk::ListStore> model;

	Task_accessor task_accessor;
	Task_id       parent_id{0};
};

}

#endif /* PARENTCHOOSER_H_ */
