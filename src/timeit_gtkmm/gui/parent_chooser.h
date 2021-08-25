#ifndef PARENT_CHOOSER_H
#define PARENT_CHOOSER_H

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
	explicit parent_chooser_widget(database& db);

	void              set_id(const task_id& id);
	void              set_parent(optional<task_id> id);
	optional<task_id> get_parent_id() const;
private:
	//Signal handlers:
	virtual void on_combo_changed();

	void populate(string& base_string, optional<task_id> parent);

	Gtk::TreeModel::iterator find_row(const task_id& id);

	//Tree model columns:
	class model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		model_columns()
		{
			add(col_id);
			add(col_name);
		}

		Gtk::TreeModelColumn<task_id>        col_id;
		Gtk::TreeModelColumn<Glib::ustring>  col_name;
	};

	model_columns columns;

	Glib::RefPtr<Gtk::ListStore> model;

	task_accessor tasks;
	optional<task_id>       parent_id;
};

}

#endif /* PARENT_CHOOSER_H */
