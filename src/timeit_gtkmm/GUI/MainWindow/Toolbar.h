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
#include "ActionObserver.h"
namespace GUI
{

class Toolbar : public Gtk::Toolbar
{
public:
	Toolbar();
	virtual ~Toolbar();
	void attach(ActionObserver*);
	void detach(ActionObserver*);
	void setTaskIsSelected(bool);
private:
	Gtk::ToolButton  m_startButton;
	Gtk::ToolButton  m_stopButton;
	Gtk::ToolButton  m_addButton;
	Gtk::ToolButton  m_editButton;
	Gtk::ToolButton  m_removeButton;
	Gtk::SeparatorToolItem  toolbutton2;
	Gtk::SeparatorToolItem  toolbutton3;
	void on_add_clicked();
	void on_start_clicked();
	void on_stop_clicked();
	void on_edit_clicked();
	void on_remove_clicked();
	std::list<ActionObserver*> observers;
};

}
#endif /* TOOLBAR_H_ */
