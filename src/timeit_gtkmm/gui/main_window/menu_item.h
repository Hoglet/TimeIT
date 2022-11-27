

#ifndef MENU_ITEM_H
#define MENU_ITEM_H


#include <string>
#include <functional>

class menu_item
{
public:
	menu_item();
	menu_item( const std::string           title,
			   std::function< void(void) > lambda
	);

	menu_item( const std::string           title,
			   const std::string           accelerator,
			   std::function< void(void) > lambda
			   );
	std::string xml();

	const std::string title;
	const std::string accelerator;
	const std::function< void(void) > lambda;
	const bool is_separator;
};


#endif //MENU_ITEM_H
