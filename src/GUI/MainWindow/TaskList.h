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
#include "ExtendedTask.h"
#include "IActionObserver.h"
#include <Database.h>

namespace GUI
{
class TaskList: public Gtk::TreeView, public DB::TaskAccessorObserver
{
public:
	TaskList(std::shared_ptr<DB::IDatabase>& database);
	virtual ~TaskList();
	void populate (Gtk::TreeModel::Row* parent=0, int parentID=0);
	int64_t getSelectedID();
	virtual void on_taskAdded(int64_t);
	virtual void on_taskUpdated(int64_t);
	virtual void on_taskRemoved(int64_t);
	virtual void on_taskParentChanged(int64_t);
	virtual void on_completeUpdate();

	void on_row_expanded(const Gtk::TreeModel::iterator& iter,const Gtk::TreeModel::Path& path);
	void on_row_collapsed(const Gtk::TreeModel::iterator& iter,const Gtk::TreeModel::Path& path);
	void attach(IActionObserver* observer);
	void detach(IActionObserver* observer);
private:
	void on_selection_changed();
	void empty();
	Gtk::TreeModel::iterator subSearch(int id,Gtk::TreeModel::Children children);
	Gtk::TreeModel::iterator findRow(int id);
	Glib::RefPtr<Gtk::TreeStore>    treeModel;
	class	ModelColumns:	public	Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns ()
		{

			add (col_id);
			add (col_name);
			add (col_pixbuf);
			add (col_time);
		};
		Gtk::TreeModelColumn <int>   col_id;
		Gtk::TreeModelColumn <Glib::ustring>    col_name;
		Gtk::TreeModelColumn <Glib::RefPtr<Gdk::Pixbuf> > 		col_pixbuf;
		Gtk::TreeModelColumn <Glib::ustring>    col_time;
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
	virtual bool on_button_press_event(GdkEventButton* event);
	void assignValuesToRow(Gtk::TreeModel::Row& row,const DB::ExtendedTask& task);
	Glib::RefPtr<Gdk::Pixbuf> runningIcon;
	Glib::RefPtr<Gdk::Pixbuf> blankIcon;
	void doUpdate();
	std::list<IActionObserver*> observers;
	std::shared_ptr<DB::IExtendedTaskAccessor> taskAccessor;
};
}
#endif // _TASK_LIST_HPP_
