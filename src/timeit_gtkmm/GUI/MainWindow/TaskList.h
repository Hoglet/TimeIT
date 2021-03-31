/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * TimeIT
 * Copyright (C) Kent Asplund 2008 <hoglet@solit.se>
 *
 * TimeIT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TimeIT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TASK_LIST_HPP_
#define _TASK_LIST_HPP_

#include <gtkmm.h>
#include <libtimeit/db/extended_task.h>
#include "ActionObserver.h"
#include <libtimeit/db/database.h>
#include <libtimeit/logic/TimeKeeper.h>
#include <libtimeit/db/extended_task_accessor.h>

namespace GUI
{
using namespace libtimeit;

class TaskList: public Gtk::TreeView, public Event_observer
{
public:
	TaskList(Database &database, ITimeKeeper &timeKeeper, Notifier &notifier);
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
	void attach(ActionObserver *observer);
	void detach(ActionObserver *observer);
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
	std::list<ActionObserver*> observers;
	Extended_task_accessor taskAccessor;
	ITimeKeeper& m_timeKeeper;
};
}
#endif // _TASK_LIST_HPP_
