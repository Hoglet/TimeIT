#include "EditTaskDialog.h"
#include "ParentChooser.h"
#include <iostream>
#include <libtimeit/logic/workspace.h>
#include <libtimeit/utils.h>

namespace GUI
{
static const int BORDER_WIDTH = 5;
static const int PADDING = 3;

using namespace libtimeit;
using namespace std;
Edit_task_dialog::Edit_task_dialog(Database &database) :
		parent_chooser(database),
		auto_track_accessor(database),
		task_accessor(database)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);



	/* This txt is the headline for the area where you choose
	 on what workspace the task should be automatically started */
	string text = string_printf("<b>%s</b>", _("Workspace tracking"));
	label1.set_text(text);

	create_layout();
	ok_button.signal_clicked().connect(sigc::mem_fun(this, &Edit_task_dialog::on_OK_button_clicked));
	cancel_button.signal_clicked().connect(sigc::mem_fun(this, &Edit_task_dialog::on_cancel_button_clicked));
	task_name_entry.signal_changed().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
	parent_chooser.signal_changed().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));

	auto iter = check_button.begin();
	while (iter != check_button.end())
	{
		(*iter)->signal_toggled().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
		++iter;
	}
}



void Edit_task_dialog::create_layout()
{
	auto layout = workspace.layout();
	auto num_rows    = layout.rows;
	auto num_columns = layout.columns;
	auto number_of_workspaces = layout.number_of_workspaces;

	workspace_table.resize(num_rows, num_columns);
	table2.resize(3, 2);
	task_name_entry.set_flags(Gtk::CAN_FOCUS);
	horizontal_box_1.pack_start(parent_label, Gtk::PACK_SHRINK, 0);
	horizontal_box_1.pack_start(parent_chooser);
	horizontal_box_2.pack_start(name_label, Gtk::PACK_SHRINK, 0);
	horizontal_box_2.pack_start(task_name_entry);

	label1.set_use_markup(true);
	desktop_frame.add(workspace_table);
	desktop_frame.set_label_widget(label1);
	table2.set_row_spacings(3);
	table2.set_col_spacings(3);
	table2.attach(horizontal_box_2, 0, 1, 0, 1);
	table2.attach(horizontal_box_3, 0, 1, 1, 2);
	table2.attach(desktop_frame, 0, 1, 2, 3);
	cancel_button.set_flags(Gtk::CAN_FOCUS);
	ok_button.set_sensitive(false);
	ok_button.set_flags(Gtk::CAN_FOCUS);
	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(cancel_button);
	get_action_area()->pack_start(ok_button);
	get_vbox()->set_spacing(2);
	get_vbox()->pack_start(horizontal_box_1, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(table2, Gtk::PACK_EXPAND_WIDGET, PADDING);
	set_border_width(BORDER_WIDTH);
	property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
	set_has_separator(false);

	for (unsigned row = 0; row < num_rows; row++)
	{
		for (unsigned column = 0; column < num_columns; column++)
		{
			if ((row * num_columns + column) < number_of_workspaces)
			{
				string button_text = workspace.name(row * num_columns + column);
				auto *new_workspace = new class Gtk::CheckButton(button_text);
				new_workspace->set_flags(Gtk::CAN_FOCUS);
				new_workspace->set_mode(true);
				workspace_table.attach(*new_workspace, column, column + 1, row, row + 1);
				check_button.push_back(new_workspace);
			}
		}
	}

	show_all_children();
}

vector<unsigned> Edit_task_dialog::get_ticked_workspaces()
{
	vector<unsigned> workspace_list;

	for (unsigned int i = 0; i < check_button.size(); i++)
	{
		try
		{
			auto* workspace_check_button = check_button.at(i);
			if (workspace_check_button->get_active())
			{
				workspace_list.push_back(i);
			}
		}
		catch (...)
		{
			cerr << " vector (checkbutton) contains less than size() says!\n";
		}
	}
	return workspace_list;
}

void Edit_task_dialog::set_task_id(Task_ID ID)
{
	task_id = ID;
	auto task = task_accessor.by_ID(task_id);
	if (task.has_value())
	{
		name = task->name;
		set_parent(task->parent_ID);
		task_name_entry.set_text(name);
		vector<unsigned> workspace_list = auto_track_accessor.workspaces(ID);
		set_ticked_workspaces(workspace_list);
		ok_button.set_sensitive(false);
		workspaces = workspace_list;
		parent_chooser.setID(ID);
	}
}

void Edit_task_dialog::set_parent(Task_ID ID)
{
	parent_id = ID;
	parent_chooser.setParentID(parent_id);
}

void Edit_task_dialog::set_ticked_workspaces(vector<unsigned> workspaces_list)
{

	auto check_button_iter = check_button.begin();
	while (check_button_iter != check_button.end())
	{
		(*check_button_iter)->set_active(false);
		check_button_iter++;
	}
	auto iter = workspaces_list.begin();
	while (iter != workspaces_list.end())
	{
		try
		{
			check_button.at(*iter)->set_active(true);
		}
		catch (...)
		{
			cerr << "#workspace != #checkbutton!\n";
		}
		iter++;
	}

}
void Edit_task_dialog::on_OK_button_clicked()
{

	stringstream check_button_name;
	auto workspace_list = get_ticked_workspaces();
	auto new_name = task_name_entry.get_text();
	parent_id = parent_chooser.getParentID();

	if (task_id < 1)
	{
		Task task(new_name, parent_id);
		task_id = task_accessor.create(task);
	}
	else
	{
		task_accessor.set_parent_id(task_id, parent_id);
		auto task = task_accessor.by_ID(task_id);
		if (task.has_value())
		{
			task_accessor.update(task->with_name(new_name));
		}
	}
	auto_track_accessor.set_workspaces(task_id, workspace_list);
	task_id = -1;
	parent_id = 0;
	hide();
}
void Edit_task_dialog::on_cancel_button_clicked()
{
	task_id = -1;
	parent_id = 0;
	hide();
}
void Edit_task_dialog::check_for_changes()
{
	auto ticked_workspaces = get_ticked_workspaces();

	string entry_name = task_name_entry.get_text();
	if ( name != entry_name || ticked_workspaces != workspaces || parent_chooser.getParentID() != parent_id)
	{
		ok_button.set_sensitive(true);
	}
	else
	{
		ok_button.set_sensitive(false);
	}
}

void Edit_task_dialog::on_data_changed()
{
	check_for_changes();
}

Edit_task_dialog::~Edit_task_dialog()
{
	auto iter = check_button.end();
	for (; iter != check_button.end(); iter++)
	{
		Gtk::CheckButton *button = *iter;
		delete button;
	}

}

void Edit_task_dialog::get_position(int &Window_x, int &Window_y)
{
	Gtk::Dialog::get_position(Window_x, Window_y);
}

bool Edit_task_dialog::is_visible()
{
	return Gtk::Dialog::is_visible();
}

void Edit_task_dialog::move(int x, int y)
{
	Gtk::Dialog::move(x,y);
}

void Edit_task_dialog::hide()
{
	Gtk::Dialog::hide();
}

void Edit_task_dialog::show()
{
	Gtk::Dialog::show();
}

}
