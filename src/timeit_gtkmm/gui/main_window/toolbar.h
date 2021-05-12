/*
 * toolbar.h
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#ifndef TOOLBAR_H_
#define TOOLBAR_H_

#include <gtkmm.h>
#include <list>
#include "action_observer.h"
namespace gui
{

class Toolbar : public Gtk::Toolbar
{
public:
	Toolbar();
	void attach(Action_observer*);
	void detach(Action_observer*);
	void set_task_is_selected(bool sensitive);
private:
	Gtk::ToolButton  start_button;
	Gtk::ToolButton  stop_button;
	Gtk::ToolButton  add_button;
	Gtk::ToolButton  edit_button;
	Gtk::ToolButton  remove_button;
	Gtk::SeparatorToolItem  toolbutton_2;
	Gtk::SeparatorToolItem  toolbutton_3;
	void on_add_clicked();
	void on_start_clicked();
	void on_stop_clicked();
	void on_edit_clicked();
	void on_remove_clicked();
	std::list<Action_observer*> observers;
};

}
#endif /* TOOLBAR_H_ */
