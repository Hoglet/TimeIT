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

class parent_chooser_widget: public Gtk::ComboBox
{
public:
	parent_chooser_widget(database& db);

	void    set_id(Task_id id);
	void    set_parent(Task_id id);
	Task_id get_parent_id() const;
private:
	//Signal handlers:
	virtual void on_combo_changed();
	void populate(string& base_string, Task_id parent_id);

	Gtk::TreeModel::iterator find_row(Task_id id);

	//Tree model columns:
	class model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		model_columns()
		{
			add(col_id);
			add(col_name);
		}

		Gtk::TreeModelColumn<Task_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
	};

	model_columns columns;

	Glib::RefPtr<Gtk::ListStore> model;

	task_accessor tasks;
	Task_id       parent_id{0};
};

}

#endif /* PARENTCHOOSER_H_ */
