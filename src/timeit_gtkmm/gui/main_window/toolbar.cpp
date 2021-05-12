/*
 * toolbar.cpp
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#include "toolbar.h"

namespace gui
{

Toolbar::Toolbar() :
		start_button(Gtk::StockID("gtk-media-play")), stop_button(Gtk::StockID("gtk-stop")), add_button(Gtk::StockID("gtk-new")),
		edit_button(Gtk::StockID("gtk-edit")), remove_button(Gtk::StockID("gtk-delete"))
{

	append(toolbutton_2);
	append(start_button);
	append(stop_button);
	append(toolbutton_3);
	append(add_button);
	append(edit_button);
	append(remove_button);

	add_button.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_add_clicked));
	edit_button.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_edit_clicked));
	start_button.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_start_clicked));
	stop_button.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_stop_clicked));
	remove_button.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_remove_clicked));

	set_task_is_selected(false);
}

void Toolbar::set_task_is_selected(bool sensitive)
{
	start_button.set_sensitive(sensitive);
	stop_button.set_sensitive(sensitive);
	edit_button.set_sensitive(sensitive);
	remove_button.set_sensitive(sensitive);
}

void Toolbar::on_add_clicked()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_add_task();
	}
}

void Toolbar::on_start_clicked()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_start_task();
	}
}

void Toolbar::on_stop_clicked()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_stop_task();
	}
}

void Toolbar::on_edit_clicked()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_edit_task();
	}
}

void Toolbar::on_remove_clicked()
{
	std::list<Action_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Action_observer* observer = *iter;
		observer->on_action_remove_task();
	}
}


void Toolbar::attach(Action_observer* observer)
{
	observers.push_back(observer);
}
void Toolbar::detach(Action_observer* observer)
{
	observers.remove(observer);
}
}
