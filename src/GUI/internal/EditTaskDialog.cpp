#include "EditTaskDialog.h"
#include "ParentChooser.h"
#include <sstream>
#include <iostream>
#include <Workspace.h>
#include <glibmm/i18n.h>

namespace GUI
{
namespace Internal
{
using namespace DB;
using namespace std;
EditTaskDialog::EditTaskDialog(boost::shared_ptr<DB::Database>& database) :
	CancelButton(Gtk::StockID("gtk-revert-to-saved")), OKButton(Gtk::StockID("gtk-apply")), parentChooser(database)
{
	parentID = 0;
	autoTrackAccessor = database->getAutotrackAccessor();
	taskAccessor = database->getTaskAccessor();
	taskID = 0;
	numRows = workspace.get_numberOfRows();
	numColumns = workspace.get_numberOfColumns();
	numberOfWorkspaces = workspace.get_numberOfWorkspaces();

	parentLabel.set_text(_("Parent: "));
	NameLabel.set_text(_("Name: "));
	label1.set_text(_("<b>Desktop</b>"));
	AutotrackButton.set_label(_("Desktop tracking"));

	createLayout();
	OKButton.signal_clicked().connect(sigc::mem_fun(this, &EditTaskDialog::on_OKButton_clicked));
	CancelButton.signal_clicked().connect(sigc::mem_fun(this, &EditTaskDialog::on_CancelButton_clicked));
	taskNameEntry.signal_changed().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));
	AutotrackButton.signal_toggled().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));
	parentChooser.signal_changed().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));

	std::vector<Gtk::CheckButton*>::iterator iter=checkbutton.begin();
	while (iter != checkbutton.end())
	{
		(*iter)->signal_toggled().connect(sigc::mem_fun(this, &EditTaskDialog::on_data_changed));
		iter++;
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
	AutotrackButton.set_flags(Gtk::CAN_FOCUS);
	AutotrackButton.set_mode(true);
	hbox3.pack_start(AutotrackButton, Gtk::PACK_SHRINK, 0);

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
				Gtk::CheckButton* newWorkspace = new class Gtk::CheckButton(buttonText);
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
	Gtk::CheckButton* workspaceCheckButton;
	std::vector<int> workspaces;

	for(unsigned int i = 0; i< checkbutton.size(); i++)
	{
		try
		{
			workspaceCheckButton = checkbutton.at(i);
			if (workspaceCheckButton->get_active())
			{
				workspaces.push_back(i);
			}
		}
		catch(...)
		{
			cerr<< " vector (checkbutton) contains less than size() says!\n";
		}
	}
	return workspaces;
}

void EditTaskDialog::setTaskID(int64_t ID)
{
	taskID = ID;
	Task task = taskAccessor->getTask(taskID);
	name = task.getName();
	setParent( task.getParentID() );
	autotrack = task.getAutotrack();
	taskNameEntry.set_text(name);
	AutotrackButton.set_active(autotrack);
	std::vector<int> workspaces = autoTrackAccessor->getWorkspaces(ID);
	setTickedWorkspaces(workspaces);
	OKButton.set_sensitive(false);
	workspaces = workspaces;
	parentChooser.setID(ID);
}

void EditTaskDialog::setParent(int ID)
{
	parentID = ID;
	parentChooser.setParentID(parentID);
}

void EditTaskDialog::setTickedWorkspaces(std::vector<int> workspaces)
{

	std::vector<Gtk::CheckButton*>::iterator chbiter = checkbutton.begin();
	while(chbiter!=checkbutton.end())
	{
		(*chbiter)->set_active(false);
		chbiter++;
	}
	std::vector<int>::iterator iter = workspaces.begin();
	while( iter != workspaces.end())
	{
		try
		{
			checkbutton.at(*iter)->set_active(true);
		}
		catch(...)
		{
			cerr<<"#workspace != #checkbutton!\n";
		}
		iter++;
	}

}
void EditTaskDialog::on_OKButton_clicked()
{
	bool autotrack;
	std::string name;
	std::stringstream checkButtonName;
	std::vector<int> workspaces = getTickedWorkspaces();
	name = taskNameEntry.get_text();
	autotrack = AutotrackButton.get_active();
	parentID = parentChooser.getParentID();

	if(taskID<1)
	{
		taskID=taskAccessor->newTask(name, parentID);
	}
	else
	{
		taskAccessor->setParentID(taskID, parentID);
		taskAccessor->setTaskName(taskID, name);
	}
	autoTrackAccessor->setWorkspaces(taskID, workspaces);
	taskAccessor->setAutotrack(taskID, autotrack);
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
	if (taskNameEntry.get_text() != name || AutotrackButton.get_active() != autotrack || tickedWorkspaces != workspaces
			|| parentChooser.getParentID() != parentID)
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
		Gtk::CheckButton* button = *iter;
		delete button;
	}

}
}
}
