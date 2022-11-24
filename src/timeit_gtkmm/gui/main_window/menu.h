

#ifndef MENU_H
#define MENU_H


#include "submenu.h"

class menu
{

public:
	menu &add( const submenu& menu );
	Gtk::Widget* get_menu_bar();
	const Glib::RefPtr<Gio::ActionGroup> get_actions();
private:
	std::list<submenu> submenus;
	std::string        submenus_xml();
};


#endif //MENU_H
