/*
 * Details.h
 *
 *  Created on: 2008-jul-22
 *      Author: hoglet
 */

#ifndef DETAILS_H_
#define DETAILS_H_

#include <gtkmm.h>
#include <signal.h>

#include "MainWindow/Summary.h"

#include <libtimeit/db/database.h>
#include <libtimeit/db/time_accessor.h>
#include <libtimeit/db/settings_accessor.h>

#include <vector>

namespace GUI
{
using namespace libtimeit;
using namespace std;

class DetailsObserver
{
public:
	virtual ~DetailsObserver()
	{
	}
	;
	virtual void on_selected_changed() = 0;
	virtual void on_edit_details(int64_t) = 0;
};


struct Row_data
{
	int64_t time_ID;
	time_t  prev_start;
	time_t  start;
	time_t  stop;
	time_t  next_start;
	bool    first_in_day;
	bool    last_in_day;
	bool    running ;
	int     cumulative_time;
};


/*
 *  Is showing details on the task that is selected
 *  (if any) in active (day/week/month)-summary
 */
class Details: public Gtk::TreeView, public Event_observer
{
public:
	Details(Database &database, Notifier& notifier);
	~Details() = default;
	void set(int64_t ID, time_t startTime, time_t stopTime);
	bool on_button_press_event(GdkEventButton *event);
	void on_menu_file_popup_edit();
	void on_menu_file_popup_remove();
	void on_menu_file_popup_merge();
	void on_menu_file_popup_split();
	//EventObserver interface
	virtual void on_task_added(int64_t)
	{
	}
	;
	virtual void on_task_updated(int64_t);
	virtual void on_task_removed(int64_t);
	virtual void on_complete_update();
	virtual void on_task_name_changed(int64_t);
	virtual void on_time_entry_changed(int64_t);
	//
	int64_t getSelectedID();
	std::vector<int64_t> getSelectedAndNextID();
	//
	void attach(DetailsObserver*);
	void detach(DetailsObserver*);
private:
	void on_selection_changed();
	void update_row(Gtk::TreeModel::Row& row, Row_data );

	optional<Gtk::TreeModel::Row> findRow(int id);

	list<Row_data> create_row_data(time_t start, time_t stop);
	void           populate(list<Row_data> data_rows);
	void           update(list<Row_data> data_rows);

	Glib::RefPtr<Gtk::ListStore> m_treeModel;
	class ModelColumns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{

			add(m_col_id);
			add(m_col_date);
			add(m_col_time);
			add(m_col_time_amount);
			add(m_col_day_total);
			add(m_col_idle);
			add(m_col_morning);
		}
		;
		Gtk::TreeModelColumn<int> m_col_id;
		Gtk::TreeModelColumn<Glib::ustring> m_col_date;
		Gtk::TreeModelColumn<Glib::ustring> m_col_time;
		Gtk::TreeModelColumn<Glib::ustring> m_col_time_amount;
		Gtk::TreeModelColumn<Glib::ustring> m_col_day_total;
		Gtk::TreeModelColumn<Glib::ustring> m_col_idle;
		Gtk::TreeModelColumn<Glib::ustring> m_col_morning;
	};
	ModelColumns m_columns;
	int m_morningColumnN;
	int64_t m_taskID;

	time_t m_startTime;
	time_t m_stopTime;
	Gtk::Menu m_Menu_Popup;
	Glib::RefPtr<Gtk::MenuItem> m_merge_menu_item;
	Glib::RefPtr<Gtk::MenuItem> m_split_menu_item;
	std::list<DetailsObserver*> observers;
	Time_accessor     m_timeAccessor;
	Settings_accessor m_settingsAccessor;
};
}

#endif /* DETAILS_H_ */
