#ifndef SUBMENU_H
#define SUBMENU_H

#include <string>
#include <list>
#include <functional>
#include "menu_item.h"

class submenu
{
public:
	submenu( const std::string &string );
	const std::string title;
	submenu& add_item( const std::string &text, const std::string &accelerator, std::function< void(void) > lambda );
	std::string xml();
	std::string items_xml();
	std::list<menu_item> menu_items;
	void items();
private:
};


#endif //SUBMENU_H
