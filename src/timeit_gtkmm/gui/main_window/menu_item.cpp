

#include <fmt/format.h>
#include "menu_item.h"
#include <glibmm/markup.h>

using namespace Glib::Markup;
using namespace std;

menu_item::menu_item(
		   const std::string            op_title,
		   const std::string            op_accelerator,
		   std::function< void(void) >  op_lambda)
		   :
		   title(op_title),
		   accelerator(op_accelerator),
		   lambda(op_lambda)

{}

string escape( const string text )
{
	return escape_text( text ).data();
}


std::string menu_item::xml()
{
	return fmt::format(R"(
                    <item>
			          <attribute name='label'>{}</attribute>
			          <attribute name='action'>timeit.{}</attribute>
			          <attribute name='accel'>{}</attribute>
			        </item>
                   )", escape(title), escape(title), "" );
}
