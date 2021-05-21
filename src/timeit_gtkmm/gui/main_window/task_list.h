#ifndef TASK_LIST_HPP_
#define TASK_LIST_HPP_

#include <gtkmm.h>
#include <libtimeit/db/extended_task.h>
#include "action_observer.h"
#include <libtimeit/db/database.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>
#include <gui/images.h>

namespace gui
{
using namespace libtimeit;

class Task_list: public Gtk::TreeView, public Event_observer
{
public:
	~Task_list() override;
	Task_list(
			Database&    database,
			Time_keeper& time_keeper,
			Notifier&    notifier,
			Images&      images);
	Task_list(const Task_list& )            = delete;
	Task_list(Task_list&& )                 = delete;
	Task_list& operator=(const Task_list& ) = delete;
	Task_list& operator=(Task_list&& )      = delete;

	void populate(Gtk::TreeModel::Row *parent = nullptr, Task_id parent_id = 0);
	Task_id selected_id();
	void on_task_added(Task_id id)        override;
	void on_task_updated(Task_id id)      override;
	void on_task_removed(Task_id id)      override;
	void on_parent_changed(Task_id id)    override;
	void on_complete_update()             override;
	void on_task_name_changed(Task_id id) override;
	void on_task_time_changed(Task_id id) override;

	void on_row_expanded(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path)  override;
	void on_row_collapsed(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) override;
	void attach(Action_observer *observer);
	void detach(Action_observer *observer);
private:
	void on_selection_changed();
	void empty();
	Gtk::TreeModel::iterator sub_search(Task_id id, Gtk::TreeModel::Children children);
	Gtk::TreeModel::iterator find_row(Task_id id);
	Glib::RefPtr<Gtk::TreeStore> tree_model;
	class ModelColumns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{

			add(col_id);
			add(col_name);
			add(col_pixbuf);
			add(col_time);
		}
		;
		Gtk::TreeModelColumn<Task_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > col_pixbuf;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
	};
	ModelColumns columns;
	//
	Gtk::Menu menu_popup;
	void on_menu_start();
	void on_menu_stop();
	void on_menu_edit();
	void on_menu_add_task();
	void on_menu_remove_task();
	void on_menu_add_time();
	bool on_button_press_event(GdkEventButton *event) override;
	void assign_values_to_row(Gtk::TreeModel::Row &row, const Extended_task &task);

	Glib::RefPtr<Gdk::Pixbuf> running_icon;
	Glib::RefPtr<Gdk::Pixbuf> running_idle_icon;
	Glib::RefPtr<Gdk::Pixbuf> blank_icon;

	void do_update();
	std::list<Action_observer*> observers;
	Extended_task_accessor task_accessor;
	Time_keeper& time_keeper;
};
}
#endif // TASK_LIST_HPP_
