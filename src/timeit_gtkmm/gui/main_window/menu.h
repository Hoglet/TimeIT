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

namespace gui
{

using namespace std;

class Menu: public Gtk::MenuBar
{
public:
	Menu();
	void attach(Action_observer*);
	void detach(Action_observer*);
	void set_task_is_selected(bool show);
private:
	Gtk::Menu file_menu;
	Gtk::Menu task_menu;
	Gtk::Menu help_menu;
	std::list<Gtk::MenuItem*> task_id_dependent_menus;

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

	list<Action_observer*> observers;
};

}
#endif /* MENU_H_ */
