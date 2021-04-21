#ifndef EDIT_TASK_DIALOG_H_
#define EDIT_TASK_DIALOG_H_

#include "parent_chooser.h"
#include <gtkmm.h>
#include <string>
#include <vector>
#include <edit_task_dialog.h>
#include <libtimeit/db/autotrack_accessor.h>
#include <parent_chooser.h>
#include <IWidget.h>
#include <libtimeit/logic/workspace.h>
#include <libtimeit/db/database.h>
#include <glibmm/i18n.h>

namespace GUI
{
using namespace std;
using namespace libtimeit;

class Edit_task_dialog: public Gtk::Dialog, public IWidget
{
public:
	Edit_task_dialog(Database& database);
	Edit_task_dialog(Edit_task_dialog const&) = delete;
	Edit_task_dialog& operator = (Edit_task_dialog const&) = delete;
	Edit_task_dialog(Edit_task_dialog&&) = delete;
	Edit_task_dialog& operator=(Edit_task_dialog&&) = delete;
	~Edit_task_dialog() override;
	void on_OK_button_clicked();
	void on_cancel_button_clicked();
	void on_data_changed();
	void set_task_id(Task_ID ID);
	void set_parent(Task_ID ID);

	// IWidget interface
	void show() override;
	void hide() override;
	void move(int x, int y) override;
	bool is_visible() override;
	void get_position(int& window_x, int& window_y) override;
private:
	void             create_layout();
	void             check_for_changes();
	vector<unsigned> get_ticked_workspaces();
	void             set_ticked_workspaces(vector<unsigned> workspaces );

	Gtk::Label  name_label       {_("Name: ")};
	Gtk::HBox   horizontal_box_1;
	Gtk::HBox   horizontal_box_2;
	Gtk::HBox   horizontal_box_3;

	std::vector<Gtk::CheckButton*> check_button;
	Gtk::Table    workspace_table;
	Gtk::Label    label1;
	Gtk::Frame    desktop_frame;
	Gtk::Table    table2;

	Gtk::Button   cancel_button   {Gtk::StockID("gtk-revert-to-saved")};
	Gtk::Button   ok_button       {Gtk::StockID("gtk-apply")};
	Gtk::Entry    task_name_entry;

	// This is the Parent in the context "Parent task"
	// The parent could be be "Project A" and the children could then be "Design", "Planning", "Coding".
	// Parent task and child task.
	Gtk::Label    parent_label   {_("Parent: ")};
	ParentChooser parent_chooser;

	string           name;
	Task_ID          task_id     {0};
	Task_ID          parent_id   {0};
	vector<unsigned> workspaces;

	Auto_track_accessor auto_track_accessor;
	Task_accessor       task_accessor;
	Workspace           workspace;
};
}
#endif /*EDIT_TASK_DIALOG_H_*/
