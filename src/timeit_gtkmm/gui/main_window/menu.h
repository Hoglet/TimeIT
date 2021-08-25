#ifndef MENU_H
#define MENU_H

#include <gtkmm.h>
#include <action_observer.h>

namespace gui
{

using namespace std;

class menu_bar : public Gtk::MenuBar
{
public:
	menu_bar();
	void attach(action_observer*);
	void detach(action_observer*);
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

	list<action_observer*> observers;
};

}
#endif /* MENU_H */
