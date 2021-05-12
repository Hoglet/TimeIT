/*
 * Details.h
 *
 *  Created on: 2008-jul-22
 *      Author: hoglet
 */

#ifndef DETAILS_H_
#define DETAILS_H_

#include <gtkmm.h>
#include <csignal>

#include "main_window/summary.h"

#include <libtimeit/db/database.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/settings_accessor.h>
#include <gui_factory.h>
#include <vector>

namespace gui
{
using namespace libtimeit;
using namespace std;

class DetailsObserver
{
public:
	virtual void on_selected_changed() = 0;
	virtual void on_edit_details(int64_t) = 0;
};


struct Row_data
{
	Time_id time_id;
	time_t  prev_start;
	time_t  start;
	time_t  stop;
	time_t  next_start;
	bool    first_in_day;
	bool    last_in_day;
	bool    running ;
	time_t  cumulative_time;
};


/*
 *  Is showing details on the task that is selected
 *  (if any) in active (day/week/month)-summary
 */
class Details: public Gtk::TreeView, public Event_observer
{
public:
	Details(Database &database, Notifier& notifier, Window_manager&  gui_factory);
	void set(Task_id id, time_t start, time_t stop);
	bool on_button_press_event(GdkEventButton *event) override;
	void on_menu_file_popup_edit();
	void on_menu_file_popup_remove();
	void on_menu_file_popup_merge();
	void on_menu_file_popup_split();
	//EventObserver interface
	void on_task_updated(Task_id task_id) override;
	void on_task_removed(Task_id task_id) override;
	void on_complete_update() override;
	void on_task_name_changed(Task_id task_id) override;
	void on_time_entry_changed(Time_id id) override;
	//
	Time_id get_selected_id();
	vector<Time_id> get_selected_and_next_id();
	//

	void attach(DetailsObserver*);
	void detach(DetailsObserver*);
private:
	void on_selection_changed();
	void update_row(Gtk::TreeModel::Row& row, Row_data ) const;

	optional<Gtk::TreeModel::Row> find_row(Time_id id);

	list<Row_data> create_row_data(time_t start, time_t stop);
	void           populate(list<Row_data> data_rows);
	void           update(list<Row_data> data_rows);

	Glib::RefPtr<Gtk::ListStore> tree_model;
	class ModelColumns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{

			add(col_id);
			add(col_date);
			add(col_time);
			add(col_time_amount);
			add(col_day_total);
			add(col_idle);
			add(col_morning);
		}
		;
		Gtk::TreeModelColumn<Time_id>       col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_date;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
		Gtk::TreeModelColumn<Glib::ustring> col_time_amount;
		Gtk::TreeModelColumn<Glib::ustring> col_day_total;
		Gtk::TreeModelColumn<Glib::ustring> col_idle;
		Gtk::TreeModelColumn<Glib::ustring> col_morning;
	};
	ModelColumns columns;
	Task_id      task_id {0};

	time_t start_time {0};
	time_t stop_time  {0};
	Gtk::Menu                   menu_popup;
	Glib::RefPtr<Gtk::MenuItem> merge_menu_item;
	Glib::RefPtr<Gtk::MenuItem> split_menu_item;
	std::list<DetailsObserver*> observers;
	Time_accessor               time_accessor;
	Settings_accessor           settings_accessor;
	Window_manager&                 gui_factory;
	Database&                   database;
	void edit_time_entry(Time_id i);
};
}

#endif /* DETAILS_H_ */
