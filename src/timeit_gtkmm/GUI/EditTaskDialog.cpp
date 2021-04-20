#include "EditTaskDialog.h"
#include "ParentChooser.h"
#include <iostream>
#include <libtimeit/logic/workspace.h>
#include <glibmm/i18n.h>
#include <libtimeit/utils.h>

namespace GUI
{
using namespace libtimeit;
using namespace std;
EditTaskDialog::EditTaskDialog(Database &database) :
		CancelButton(Gtk::StockID("gtk-revert-to-saved")),
		OKButton(Gtk::StockID("gtk-apply")),
		parentChooser(database),
		autoTrackAccessor(database),
		taskAccessor(database)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	parentID = 0;
	taskID = 0;
	numRows = workspace.get_numberOfRows();
	numColumns = workspace.get_numberOfColumns();
	numberOfWorkspaces = workspace.get_numberOfWorkspaces();

	// This is the Parent in the context "Parent task"
	// The parent could be be "Project A" and the children could then be "Design", "Planning", "Coding".
	// Parent task and child task.
	parentLabel.set_text(_("Parent: "));
	NameLabel.set_text(_("Name: "));
	/* This txt is the headline for the area where you choose
	 on what workspace the task should be automatically started */
	std::string text = libtimeit::string_printf("<b>%s</b>", _("Workspace tracking"));
	label1.set_text(text);

	createLayout();
	OKButton.signal_clicked().connect(sigc::mem_fun(this, &EditTaskDialog::on_OKButton_clicked));
	CancelButton.signal_clicked().connect(sigc::mem_fun(this, &EditTaskDialog::on_CancelButton_clicked));
	taskNameEntry.signal_changed().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));
	parentChooser.signal_changed().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));

	std::vector<Gtk::CheckButton*>::iterator iter = checkbutton.begin();
	while (iter != checkbutton.end())
	{
		(*iter)->signal_toggled().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));
		++iter;
	}
}

static const int BORDER_WIDTH = 5;

static const int PADDING = 3;

void EditTaskDialog::createLayout()
{

	workspaceTable.resize(numRows, numColumns);
	table2.resize(3, 2);
	taskNameEntry.set_flags(Gtk::CAN_FOCUS);
	hbox1.pack_start(parentLabel, Gtk::PACK_SHRINK, 0);
	hbox1.pack_start(parentChooser);
	hbox2.pack_start(NameLabel, Gtk::PACK_SHRINK, 0);
	hbox2.pack_start(taskNameEntry);

	label1.set_use_markup(true);
	DesktopFrame.add(workspaceTable);
	DesktopFrame.set_label_widget(label1);
	table2.set_row_spacings(3);
	table2.set_col_spacings(3);
	table2.attach(hbox2, 0, 1, 0, 1);
	table2.attach(hbox3, 0, 1, 1, 2);
	table2.attach(DesktopFrame, 0, 1, 2, 3);
	CancelButton.set_flags(Gtk::CAN_FOCUS);
	OKButton.set_sensitive(false);
	OKButton.set_flags(Gtk::CAN_FOCUS);
	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(CancelButton);
	get_action_area()->pack_start(OKButton);
	get_vbox()->set_spacing(2);
	get_vbox()->pack_start(hbox1, Gtk::PACK_EXPAND_WIDGET, PADDING);
	get_vbox()->pack_start(table2, Gtk::PACK_EXPAND_WIDGET, PADDING);
	set_border_width(BORDER_WIDTH);
	property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
	set_has_separator(false);

	for (unsigned row = 0; row < numRows; row++)
	{
		for (unsigned column = 0; column < numColumns; column++)
		{
			if ((row * numColumns + column) < numberOfWorkspaces)
			{
				std::string buttonText = workspace.get_name(row * numColumns + column);
				Gtk::CheckButton *newWorkspace = new class Gtk::CheckButton(buttonText);
				newWorkspace->set_flags(Gtk::CAN_FOCUS);
				newWorkspace->set_mode(true);
				workspaceTable.attach(*newWorkspace, column, column + 1, row, row + 1);
				checkbutton.push_back(newWorkspace);
			}
		}
	}

	show_all_children();
}

std::vector<int> EditTaskDialog::getTickedWorkspaces()
{
	Gtk::CheckButton *workspaceCheckButton;
	std::vector<int> workspaces;

	for (unsigned int i = 0; i < checkbutton.size(); i++)
	{
		try
		{
			workspaceCheckButton = checkbutton.at(i);
			if (workspaceCheckButton->get_active())
			{
				workspaces.push_back(i);
			}
		}
		catch (...)
		{
			cerr << " vector (checkbutton) contains less than size() says!\n";
		}
	}
	return workspaces;
}

void EditTaskDialog::setTaskID(int64_t ID)
{
	taskID = ID;
	auto task = taskAccessor.by_ID(taskID);
	if (task.has_value())
	{
		name = task->name;
		setParent(task->parent_ID);
		taskNameEntry.set_text(name);
		std::vector<int> workspaces = autoTrackAccessor.workspaces(ID);
		setTickedWorkspaces(workspaces);
		OKButton.set_sensitive(false);
		this->workspaces = workspaces;
		parentChooser.setID(ID);
	}
}

void EditTaskDialog::setParent(int ID)
{
	parentID = ID;
	parentChooser.setParentID(parentID);
}

void EditTaskDialog::setTickedWorkspaces(vector<int> workspaces)
{

	vector<Gtk::CheckButton*>::iterator check_button_iter = checkbutton.begin();
	while (check_button_iter != checkbutton.end())
	{
		(*check_button_iter)->set_active(false);
		check_button_iter++;
	}
	std::vector<int>::iterator iter = workspaces.begin();
	while (iter != workspaces.end())
	{
		try
		{
			checkbutton.at(*iter)->set_active(true);
		}
		catch (...)
		{
			cerr << "#workspace != #checkbutton!\n";
		}
		iter++;
	}

}
void EditTaskDialog::on_OKButton_clicked()
{
	string name;
	stringstream checkButtonName;
	vector<int> workspaces = getTickedWorkspaces();
	name = taskNameEntry.get_text();
	parentID = parentChooser.getParentID();

	if (taskID < 1)
	{
		Task task(name,parentID);
		taskID = taskAccessor.create(task);
	}
	else
	{
		taskAccessor.setParentID(taskID, parentID);
		auto task = taskAccessor.by_ID(taskID);
		if (task.has_value())
		{
			taskAccessor.update(task->with_name(name));
		}
	}
	autoTrackAccessor.set_workspaces(taskID, workspaces);
	taskID = -1;
	parentID = 0;
	hide();
}
void EditTaskDialog::on_CancelButton_clicked()
{
	taskID = -1;
	parentID = 0;
	hide();
}
void EditTaskDialog::check4changes()
{
	vector<int> tickedWorkspaces = getTickedWorkspaces();

	string entry_name = taskNameEntry.get_text();
	if ( name != entry_name  || tickedWorkspaces != workspaces || parentChooser.getParentID() != parentID)
	{
		OKButton.set_sensitive(true);
	}
	else
	{
		OKButton.set_sensitive(false);
	}
}

void EditTaskDialog::on_data_changed()
{
	check4changes();
}

EditTaskDialog::~EditTaskDialog()
{
	vector<Gtk::CheckButton*>::iterator iter = checkbutton.end();
	for (; iter != checkbutton.end(); iter++)
	{
		Gtk::CheckButton *button = *iter;
		delete button;
	}

}
}
