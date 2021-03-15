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

#include <libtimeit/db/Database.h>
#include <libtimeit/db/TimeAccessor.h>

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
/*
 *  Is showing details on the task that is selected
 *  (if any) in active (day/week/month)-summary
 */
class Details: public Gtk::TreeView, public EventObserver
{
public:
	Details(shared_ptr<IDatabase> &database);
	virtual ~Details();
	void set(int64_t ID, time_t startTime, time_t stopTime);
	void on_selectedTaskChanged(Summary&);
	bool on_button_press_event(GdkEventButton *event);
	void on_menu_file_popup_edit();
	void on_menu_file_popup_remove();
	//EventObserver interface
	virtual void on_taskAdded(int64_t)
	{
	}
	;
	virtual void on_taskUpdated(int64_t);
	virtual void on_taskRemoved(int64_t);
	virtual void on_completeUpdate();
	virtual void on_taskNameChanged(int64_t);
	virtual void on_taskTimeChanged(int64_t);
	//
	int64_t getSelectedID();
	//
	void attach(DetailsObserver*);
	void detach(DetailsObserver*);
private:
	void on_selection_changed();
	void on_dateChanged();
	Gtk::TreeModel::iterator findRow(int id);
	//void taskChanged(Task&);
	void taskUpdated(int64_t taskID);
	void populate();
	void empty();

	Glib::RefPtr<Gtk::ListStore> m_treeModel;
	class ModelColumns: public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{

			add(m_col_id);
			add(m_col_time);
			add(m_col_idle);
			add(m_col_morning);
			add(m_col_evening);
		}
		;
		Gtk::TreeModelColumn<int> m_col_id;
		Gtk::TreeModelColumn<Glib::ustring> m_col_time;
		Gtk::TreeModelColumn<Glib::ustring> m_col_idle;
		Gtk::TreeModelColumn<Glib::ustring> m_col_morning;
		Gtk::TreeModelColumn<Glib::ustring> m_col_evening;
	};
	ModelColumns m_columns;
	int m_morningColumnN;
	Gtk::Calendar *m_calendar;
	int64_t m_taskID;
//	time_t m_activeDay;
	time_t m_startTime;
	time_t m_stopTime;
	Gtk::Menu m_Menu_Popup;
	std::list<DetailsObserver*> observers;
	shared_ptr<ITimeAccessor> m_timeAccessor;
	shared_ptr<ITaskAccessor> m_taskAccessor;
};
}

#endif /* DETAILS_H_ */
