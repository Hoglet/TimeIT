#ifndef DETAILS_H
#define DETAILS_H

#include <gtkmm.h>
#include <csignal>

#include "main_window/summary.h"

#include <libtimeit/db/database.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <window_manager.h>
#include <vector>

namespace gui
{
using namespace libtimeit;
using namespace std;

struct row_data
{
	time_id id;
	time_point<system_clock> prev_start;
	time_point<system_clock> start;
	time_point<system_clock> stop;
	time_point<system_clock> next_start;
	bool    first_in_day;
	bool    last_in_day;
	bool    running ;
	seconds cumulative_time;
	string  comment;
};


/*
 *  Is showing details on the task that is selected
 *  (if any) in active (day/week/month)-summary
 */
class details: public Gtk::TreeView, public event_observer, public summary_observer
{
public:
	details( database &db, notification_manager& notifier, window_manager&  gui_factory);
	void set(
			optional<task_id>        id,
			time_point<system_clock> start,
			time_point<system_clock> stop);

	bool on_button_press_event(GdkEventButton *event) override;
	void on_menu_file_popup_edit();
	void on_menu_file_popup_remove();
	void on_menu_file_popup_merge();
	void on_menu_file_popup_split();
	//EventObserver interface
	void on_task_updated(const task_id& id) override;
	void on_task_removed(const task& item) override;
	void on_complete_update() override;
	void on_task_name_changed(const task& item) override;
	void on_time_entry_changed(const time_entry& /*te*/) override;
	//Summary_observer interface
	void on_selection_changed(
			optional<task_id>        id,
			time_point<system_clock> start_time,
			time_point<system_clock> stop_time) override;
	//
	optional<time_id>   get_selected();
	list<time_id>       get_selected_and_next();
	//
private:
	void update_row(Gtk::TreeModel::Row& row, row_data ) const;

	optional<Gtk::TreeModel::Row> find_row(time_id id);

	list<row_data> create_row_data( time_point<system_clock> start, time_point<system_clock> stop);

	void           populate();
	void           populate(list<row_data> data_rows);
	void           update(list<row_data> data_rows);

	Glib::RefPtr<Gtk::ListStore> tree_model;
	class model_columns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		model_columns()
		{

			add(col_id);
			add(col_date);
			add(col_time);
			add(col_time_amount);
			add(col_day_total);
			add(col_idle);
			add(col_morning);
			add(col_comment);
		}
		;
		Gtk::TreeModelColumn<time_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_date;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
		Gtk::TreeModelColumn<Glib::ustring> col_time_amount;
		Gtk::TreeModelColumn<Glib::ustring> col_day_total;
		Gtk::TreeModelColumn<Glib::ustring> col_idle;
		Gtk::TreeModelColumn<Glib::ustring> col_morning;
		Gtk::TreeModelColumn<Glib::ustring> col_comment;
	};
	model_columns columns;
	optional<task_id>      presented_task;

	time_point<system_clock> start_time { system_clock::from_time_t(0) };
	time_point<system_clock> stop_time  { system_clock::from_time_t(0) };
	Gtk::Menu                   menu_popup;
	Glib::RefPtr<Gtk::MenuItem> merge_menu_item;
	Glib::RefPtr<Gtk::MenuItem> split_menu_item;
	time_accessor               times;
	settings_accessor           settings;
	window_manager&             windows;
	database&                   db;
	void edit_time_entry(time_id /*id*/);
};
}

#endif /* DETAILS_H */
