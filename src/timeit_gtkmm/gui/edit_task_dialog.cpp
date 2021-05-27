#include "edit_task_dialog.h"
#include "parent_chooser.h"
#include <iostream>
#include <libtimeit/logic/workspace.h>
#include <libtimeit/utils.h>
#include <libtimeit/db/default_values.h>
#include <fmt/core.h>

namespace gui
{
static const int BORDER_WIDTH            = 5;
static const int PADDING                 = 3;
static const int MAX_IDLE_TIME           = 500;
static const int IDLE_TIME_PAGING_LENGTH = 10;

using namespace libtimeit;
using namespace std;
Edit_task_dialog::Edit_task_dialog(Database &database) :
		parent_chooser(database),
		auto_track_accessor(database),
		task_accessor(database),
		settings_accessor( database)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);



	/* This txt is the headline for the area where you choose
	 on what workspace the task should be automatically started */
	string text = fmt::format("<b>{}</b>", _("Workspace tracking"));
	workspace_label.set_text(text);

	create_layout();
	ok_button.signal_clicked().connect(sigc::mem_fun(this, &Edit_task_dialog::on_ok_button_clicked));
	cancel_button.signal_clicked().connect(sigc::mem_fun(this, &Edit_task_dialog::on_cancel_button_clicked));
	task_name_entry.signal_changed().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
	parent_chooser.signal_changed().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
	idle_time_entry.signal_changed().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
	quiet_button.signal_clicked().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));

	auto iter = check_button.begin();
	while (iter != check_button.end())
	{
		(*iter)->signal_toggled().connect(sigc::mem_fun(this, &Edit_task_dialog::on_data_changed));
		++iter;
	}
}



void Edit_task_dialog::create_layout()
{
	auto layout      = workspace.layout();
	auto num_rows    = layout.rows;
	auto num_columns = layout.columns;
	auto number_of_workspaces = layout.number_of_workspaces;

	workspace_table.resize(num_rows, num_columns);

	parent_choosing_row.pack_start(parent_label, Gtk::PACK_SHRINK, 0);
	parent_choosing_row.pack_start(parent_chooser, Gtk::PACK_SHRINK, 0);

	task_name_entry.set_flags(Gtk::CAN_FOCUS);
	name_editing_row.pack_start(name_label, Gtk::PACK_SHRINK, 0);
	name_editing_row.pack_start(task_name_entry);

	idle_label.set_tooltip_text(_("This is the time of inaction necessary to trigger the end of a time span"));
	idle_time_entry.set_tooltip_text(_("This is the time of inaction necessary to trigger an end of a time span"));
	idle_time_entry.set_max_length(4);
	idle_time_entry.set_range(1, MAX_IDLE_TIME);
	idle_time_entry.set_increments(1, IDLE_TIME_PAGING_LENGTH);
	idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT );
	idle_time_entry.set_value( idle_time );
	idle_editing_row.pack_start( idle_label, Gtk::PACK_SHRINK);
	idle_editing_row.pack_start( idle_time_entry, Gtk::PACK_SHRINK);

	quiet_button.set_label( _("Quiet operation") );
	quiet_button.set_tooltip_text(_("When this is enabled there will be no interaction"));
	quiet_row.pack_start(quiet_button);

	workspace_label.set_use_markup(true);
	desktop_frame.add(workspace_table);
	desktop_frame.set_label_widget(workspace_label);


	cancel_button.set_flags(Gtk::CAN_FOCUS);
	ok_button.set_sensitive(false);
	ok_button.set_flags(Gtk::CAN_FOCUS);

	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(cancel_button);
	get_action_area()->pack_start(ok_button);

	get_vbox()->set_spacing(2);
	get_vbox()->pack_start(parent_choosing_row, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(name_editing_row, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(idle_editing_row, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(quiet_row, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(desktop_frame, Gtk::PACK_EXPAND_WIDGET, PADDING);
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
				auto button = make_shared<Gtk::CheckButton>(button_text);

				button->set_flags(Gtk::CAN_FOCUS);
				button->set_mode(true);

				button->set_tooltip_text(_("When enabled your task will start when entering this workspace and stop when exiting"));
				workspace_table.attach( *button, column, column + 1, row, row + 1);
				check_button.push_back( button);

			}
		}
	}
	for ( auto button: check_button)
	{

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
			auto workspace_check_button = check_button.at(i);
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


void Edit_task_dialog::set_task_id(Task_id ID)
{
	task_id = ID;
	auto task = task_accessor.by_id(task_id);
	if (task.has_value())
	{
		name = task->name;
		set_parent(task->parent_id);
		task_name_entry.set_text(name);
		vector<unsigned> workspace_list = auto_track_accessor.workspaces(ID);
		set_ticked_workspaces(workspace_list);
		ok_button.set_sensitive(false);
		workspaces = workspace_list;
		parent_chooser.set_id(ID);
		idle_time = task->idle;
		quiet = task->quiet;
		if(idle_time==0)
		{
			idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
		}

		idle_time_entry.set_value( idle_time);
		quiet_button.set_active( quiet);
	}
}

void Edit_task_dialog::set_parent(Task_id ID)
{
	parent_id = ID;
	parent_chooser.set_parent(parent_id);
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
void Edit_task_dialog::on_ok_button_clicked()
{

	stringstream check_button_name;
	auto workspace_list = get_ticked_workspaces();
	auto new_name = task_name_entry.get_text();
	parent_id = parent_chooser.get_parent_id();

	if (task_id < 1)
	{
		Task task(new_name, parent_id);
		task_id = task_accessor.create(task);
	}
	else
	{
		task_accessor.set_parent_id(task_id, parent_id);
		auto task = task_accessor.by_id(task_id);
		if (task.has_value())
		{
			auto new_idle_time = (unsigned)idle_time_entry.get_value_as_int();
			auto default_idle_time = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
			if(new_idle_time == default_idle_time)
			{
				new_idle_time = 0;
			}
			auto updated_task = task->
					with_name(new_name).
					with_idle(new_idle_time).
					with_quiet(quiet_button.get_active());
			task_accessor.update(updated_task);
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
	auto entry_idle = (unsigned)idle_time_entry.get_value_as_int();
	if (
			name != entry_name ||
			ticked_workspaces != workspaces ||
					parent_chooser.get_parent_id() != parent_id ||
			entry_idle != idle_time ||
			quiet_button.get_active() != quiet
			)
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
