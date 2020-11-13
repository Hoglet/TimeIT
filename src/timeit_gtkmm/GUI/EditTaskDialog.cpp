#include "EditTaskDialog.h"
#include "ParentChooser.h"
#include <sstream>
#include <iostream>
#include <Workspace.h>
#include <glibmm/i18n.h>
#include <libtimeit/Utils.h>
namespace GUI
{
using namespace DB;
using namespace std;
EditTaskDialog::EditTaskDialog(std::shared_ptr<DB::IDatabase> &database) :
		CancelButton(Gtk::StockID("gtk-revert-to-saved")), OKButton(Gtk::StockID("gtk-apply")), parentChooser(database), autoTrackAccessor(
				database->getAutotrackAccessor()), taskAccessor(database->getTaskAccessor())

{
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
	/* This text is the headline for the area where you choose
	 on what workspace the task should be automatically started */
	std::string text = Utils::string_printf("<b>%s<b>", _("Workspace tracking"));
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
	get_vbox()->pack_start(hbox1, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(table2, Gtk::PACK_EXPAND_WIDGET, 3);
	set_border_width(5);
	property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
	set_has_separator(false);

	for (int row = 0; row < numRows; row++)
	{
		for (int column = 0; column < numColumns; column++)
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
	std::shared_ptr<Task> task = taskAccessor->getTask(taskID);
	if (task)
	{
		name = task->name();
		setParent(task->parentID());
		taskNameEntry.set_text(name);
		std::vector<int> workspaces = autoTrackAccessor->getWorkspaces(ID);
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

void EditTaskDialog::setTickedWorkspaces(std::vector<int> workspaces)
{

	std::vector<Gtk::CheckButton*>::iterator chbiter = checkbutton.begin();
	while (chbiter != checkbutton.end())
	{
		(*chbiter)->set_active(false);
		chbiter++;
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
	std::string name;
	std::stringstream checkButtonName;
	std::vector<int> workspaces = getTickedWorkspaces();
	name = taskNameEntry.get_text();
	parentID = parentChooser.getParentID();

	if (taskID < 1)
	{
		taskID = taskAccessor->newTask(name, parentID);
	}
	else
	{
		taskAccessor->setParentID(taskID, parentID);
		shared_ptr<Task> task = taskAccessor->getTask(taskID);
		if (task)
		{
			taskAccessor->updateTask(task->withName(name));
		}
	}
	autoTrackAccessor->setWorkspaces(taskID, workspaces);
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
	std::vector<int> tickedWorkspaces = getTickedWorkspaces();

	if (taskNameEntry.get_text() != name || tickedWorkspaces != workspaces || parentChooser.getParentID() != parentID)
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
	std::vector<Gtk::CheckButton*>::iterator iter = checkbutton.end();
	for (; iter != checkbutton.end(); iter++)
	{
		Gtk::CheckButton *button = *iter;
		delete button;
	}

}
}
