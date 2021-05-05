#ifndef TASK_LIST_HPP_
#define TASK_LIST_HPP_

#include <gtkmm.h>
#include <libtimeit/db/extended_task.h>
#include "action_observer.h"
#include <libtimeit/db/database.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace gui
{
using namespace libtimeit;

class TaskList: public Gtk::TreeView, public Event_observer
{
public:
	TaskList(Database &database, Time_keeper &time_keeper, Notifier &notifier);
	TaskList( const TaskList& )            = delete;
	TaskList( TaskList&& )                 = delete;
	TaskList& operator=( const TaskList& ) = delete;
	TaskList& operator=( TaskList&& )      = delete;
	virtual ~TaskList();
	void populate(Gtk::TreeModel::Row *parent = 0, int parentID = 0);
	int64_t getSelectedID();
	virtual void on_task_added(int64_t);
	virtual void on_task_updated(int64_t);
	virtual void on_task_removed(int64_t);
	virtual void on_parent_changed(int64_t);
	virtual void on_complete_update();
	virtual void on_task_name_changed(int64_t);
	virtual void on_task_time_changed(int64_t);

	void on_row_expanded(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path);
	void on_row_collapsed(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path);
	void attach(action_observer *observer);
	void detach(action_observer *observer);
private:
	void on_selection_changed();
	void empty();
	Gtk::TreeModel::iterator subSearch(int id, Gtk::TreeModel::Children children);
	Gtk::TreeModel::iterator findRow(int id);
	Glib::RefPtr<Gtk::TreeStore> treeModel;
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
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > col_pixbuf;
		Gtk::TreeModelColumn<Glib::ustring> col_time;
	};
	ModelColumns columns;
	//
	Gtk::Menu Menu_Popup;
	void on_menu_start();
	void on_menu_stop();
	void on_menu_edit();
	void on_menu_add_task();
	void on_menu_remove_task();
	void on_menu_add_time();
	virtual bool on_button_press_event(GdkEventButton *event);
	void assignValuesToRow(Gtk::TreeModel::Row &row, const Extended_task &task);
	Glib::RefPtr<Gdk::Pixbuf> runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> runningIdleIcon;
	Glib::RefPtr<Gdk::Pixbuf> blankIcon;
	void doUpdate();
	std::list<action_observer*> observers;
	Extended_task_accessor taskAccessor;
	Time_keeper& m_timeKeeper;
};
}
#endif // TASK_LIST_HPP_
