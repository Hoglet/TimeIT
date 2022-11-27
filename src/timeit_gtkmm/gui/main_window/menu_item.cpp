

#include <fmt/format.h>
#include "menu_item.h"
#include <glibmm/markup.h>

using namespace Glib::Markup;
using namespace std;

menu_item::menu_item()
		:
		lambda( []{} ),
		is_separator(true)

{}

menu_item::menu_item(
		   const std::string            op_title,
		   const std::string            op_accelerator,
		   std::function< void(void) >  op_lambda)
		   :
		   title(op_title),
		   accelerator(op_accelerator),
		   lambda(op_lambda),
		   is_separator(false)

{}

string escape( const string text )
{
	return escape_text( text ).data();
}


std::string menu_item::xml()
{
	if(is_separator)
	{
		return "</section><section>";
	}
	return fmt::format(R"(
                    <item>
			          <attribute name='label'>{}</attribute>
			          <attribute name='action'>timeit.{}</attribute>
			          <attribute name='accel'>{}</attribute>
			        </item>
                   )", escape(title), escape(title), "" );
}
