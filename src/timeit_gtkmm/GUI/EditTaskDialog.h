#ifndef EDITTASKDIALOG_H_
#define EDITTASKDIALOG_H_

#include "ParentChooser.h"
#include <gtkmm.h>
#include <string>
#include <vector>
#include <EditTaskDialog.h>
#include <libtimeit/db/AutotrackAccessor.h>
#include <ParentChooser.h>
#include <IWidget.h>
#include <libtimeit/logic/Workspace.h>
#include <libtimeit/db/Database.h>

namespace GUI
{
using namespace std;
using namespace libtimeit;

class IEditTaskDialog
{
public:
	virtual ~IEditTaskDialog() {};
	virtual void setTaskID(int64_t) = 0;
};

class IAddTaskDialog
{
public:
	virtual ~IAddTaskDialog() {};
	virtual void setParent(int ID) = 0;
};

class EditTaskDialog: public Gtk::Dialog, public IEditTaskDialog, public IAddTaskDialog, public IWidget
{
public:
	EditTaskDialog( Database& database);
	virtual ~EditTaskDialog();
	void on_OKButton_clicked();
	void on_CancelButton_clicked();
	void on_data_changed();
	virtual void setTaskID(int64_t);
	virtual void setParent(int ID);

	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void createLayout();
	void check4changes();
	vector<int> getTickedWorkspaces();
	void setTickedWorkspaces( vector<int> workspaces );

	Gtk::Label NameLabel;
	Gtk::HBox hbox1;
	Gtk::HBox hbox2;
	Gtk::HBox hbox3;
	std::vector<Gtk::CheckButton*> checkbutton;
	Gtk::Table workspaceTable;
	Gtk::Label label1;
	Gtk::Frame DesktopFrame;
	Gtk::Table table2;
	Gtk::Button CancelButton;
	Gtk::Button OKButton;
	Gtk::Entry taskNameEntry;
	Gtk::Label parentLabel;
	ParentChooser parentChooser;

	string name;
	int taskID;
	int numberOfWorkspaces;
	int numColumns;
	int numRows;
	vector<int> workspaces;
	int parentID;

	AutotrackAccessor autoTrackAccessor;
	TaskAccessor      taskAccessor;
	Workspace         workspace;
};
}
#endif /*EDITTASKDIALOG_H_*/
