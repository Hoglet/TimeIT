#ifndef SUBMENU_H
#define SUBMENU_H

#include <string>
#include <list>
#include <functional>
#include <gtkmm/widget.h>
#include "menu_item.h"

class submenu
{
public:
	submenu( const std::string &string );
	const std::string title;
	submenu& add_item( const std::string &text, const std::string &accelerator, std::function< void(void) > lambda );
	submenu& add_item( );
	submenu& append( const menu_item& );

	std::string xml();
	std::string items_xml();
	std::list<menu_item> menu_items;
	std::unique_ptr<Gtk::Menu> create( );
	const Glib::RefPtr<Gio::ActionGroup> get_actions( );
};


#endif //SUBMENU_H
