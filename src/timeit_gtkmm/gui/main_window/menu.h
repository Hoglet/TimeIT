/*
 * menu.h
 *
 *  Created on: 2008-aug-20
 *      Author: hoglet
 */

#ifndef MENU_H_
#define MENU_H_

#include <gtkmm.h>
#include <action_observer.h>

namespace GUI
{

using namespace std;

class Menu: public Gtk::MenuBar
{
public:
	Menu();
	virtual ~Menu();
	void attach(action_observer*);
	void detach(action_observer*);
	void setTaskIsSelected(bool);
private:
	Gtk::Menu m_fileMenu;
	Gtk::Menu m_taskMenu;
	Gtk::Menu m_helpMenu;
	std::list<Gtk::MenuItem*> taskIDDependentMenus;

	void on_menu_quit();
	void on_menu_about();
	void on_menu_help();
	void on_menu_add_task();
	void on_menu_remove_task();
	void on_menu_add_time();
	void on_menu_start();
	void on_menu_stop();
	void on_menu_stop_all();
	void on_menu_edit();
	void on_menu_preferences();
	void on_menu_report_bug();

	list<action_observer*> observers;
};

}
#endif /* MENU_H_ */
