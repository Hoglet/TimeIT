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

class task_list_widget : public Gtk::TreeView, public event_observer
{
public:
	~task_list_widget() override;
	task_list_widget(
			database &db,
			Time_keeper &time_keeper,
			notification_manager &notifier,
			image_cache &images);
	task_list_widget(const task_list_widget &) = delete;
	task_list_widget(task_list_widget &&) = delete;
	task_list_widget &operator=(const task_list_widget &) = delete;
	task_list_widget &operator=(task_list_widget &&) = delete;

	void populate(Gtk::TreeModel::Row* parent = nullptr, optional<task_id> parent_id = {});
	optional<task_id> selected_id();
	void on_task_added(const task& item) override;
	void on_task_updated(const task_id& id) override;
	void on_task_removed(const task& item) override;
	void on_parent_changed(const task& item) override;
	void on_complete_update() override;
	void on_task_name_changed(const task& item) override;
	void on_task_time_changed(const task_id& id) override;

	void on_row_expanded(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) override;
	void on_row_collapsed(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) override;
	void attach(action_observer* observer);
	void detach(action_observer* observer);
private:
	void on_selection_changed();
	void empty();
	Gtk::TreeModel::iterator sub_search(const task_id& id, Gtk::TreeModel::Children children);
	Gtk::TreeModel::iterator find_row(const task_id& id);
	Glib::RefPtr<Gtk::TreeStore> tree_model;

	class model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		model_columns ()
		{

			add(col_id);
			add(col_name);
			add(col_pixbuf);
			add(col_time);
		};
		Gtk::TreeModelColumn<task_id> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > col_pixbuf;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
	};

	model_columns  columns;
	//
	Gtk::Menu menu_popup;
	void on_menu_start();
	void on_menu_stop();
	void on_menu_edit();
	void on_menu_add_task();
	void on_menu_remove_task();
	void on_menu_add_time();
	bool on_button_press_event(GdkEventButton* event) override;
	void assign_values_to_row(Gtk::TreeModel::Row &row, const extended_task &task_);

	Glib::RefPtr<Gdk::Pixbuf> running_icon;
	Glib::RefPtr<Gdk::Pixbuf> running_idle_icon;
	Glib::RefPtr<Gdk::Pixbuf> blank_icon;

	void do_update();
	std::list<action_observer*> observers;
	extended_task_accessor tasks;
	Time_keeper &time_keeper;
};
}
#endif // TASK_LIST_HPP_
