

#include <fmt/format.h>
#include "submenu.h"
#include "menu_item.h"

using namespace std;

submenu::submenu( const string &op_title ) : title(op_title)
{
}

submenu& submenu::add_item( const std::string &text, const std::string &accelerator, std::function< void(void) > lambda )
{
	menu_items.push_back(menu_item(text,accelerator, lambda));
	return *this;
}

string submenu::xml()
{
	return fmt::format( R"(
			    <submenu>
			      <attribute name='label'>{}</attribute>
			      <section>
                     {}
			      </section>
			    </submenu>
				)", title, items_xml()) ;
}

std::string submenu::items_xml()
{
	string result;
	for ( auto item: menu_items )
	{
		result.append( item.xml() );
	}
	return result;
}
