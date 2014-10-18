/*
 * toolbar.cpp
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#include "Toolbar.h"

namespace GUI
{
namespace Internal
{

Toolbar::Toolbar() :
	m_startButton(Gtk::StockID("gtk-media-play")), m_stopButton(Gtk::StockID("gtk-stop")), m_addButton(Gtk::StockID("gtk-new")),
			m_editButton(Gtk::StockID("gtk-edit")), m_removeButton(Gtk::StockID("gtk-delete"))
{

	append(toolbutton2);
	append(m_startButton);
	append(m_stopButton);
	append(toolbutton3);
	append(m_addButton);
	append(m_editButton);
	append(m_removeButton);

	m_addButton.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_add_clicked));
	m_editButton.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_edit_clicked));
	m_startButton.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_start_clicked));
	m_stopButton.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_stop_clicked));
	m_removeButton.signal_clicked().connect(sigc::mem_fun(this, &Toolbar::on_remove_clicked));

	setTaskIsSelected(false);
}

Toolbar::~Toolbar()
{
}
void Toolbar::setTaskIsSelected(bool sensitive)
{
	m_startButton.set_sensitive(sensitive);
	m_stopButton.set_sensitive(sensitive);
	m_editButton.set_sensitive(sensitive);
	m_removeButton.set_sensitive(sensitive);
}

void Toolbar::on_add_clicked()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver* observer = *iter;
		observer->on_action_add_task();
	}
}

void Toolbar::on_start_clicked()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver* observer = *iter;
		observer->on_action_start_task();
	}
}

void Toolbar::on_stop_clicked()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver* observer = *iter;
		observer->on_action_stop_task();
	}
}

void Toolbar::on_edit_clicked()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver* observer = *iter;
		observer->on_action_edit_task();
	}
}

void Toolbar::on_remove_clicked()
{
	std::list<IActionObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		IActionObserver* observer = *iter;
		observer->on_action_remove_task();
	}
}


void Toolbar::attach(IActionObserver* observer)
{
	observers.push_back(observer);
}
void Toolbar::detach(IActionObserver* observer)
{
	observers.remove(observer);
}
}
}
