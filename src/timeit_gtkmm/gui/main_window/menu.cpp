#include <gtkmm/menubar.h>
#include <gtkmm/builder.h>
#include <giomm/menu.h>
#include <fmt/format.h>
#include <giomm/simpleactiongroup.h>
#include "menu.h"

using namespace std;
using namespace fmt;
using namespace Gtk;

menu &menu::add( const submenu& menu )
{
	submenus.push_back( menu);
	return *this;
}

unique_ptr<MenuBar> menu::get_menu_bar()
{
	auto builder = Gtk::Builder::create();
	string ui_info = format(R"(
            <interface>
			  <menu id='menubar'>
	             {}
              </menu>
            </interface>
	)", submenus_xml());

	builder->add_from_string( ui_info);
	auto object = builder->get_object( "menubar");
	auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);

	auto result = make_unique<MenuBar>(gmenu);

	result->insert_action_group( "timeit",get_actions());
	return result;
}

string menu::submenus_xml()
{
	string result;
	for ( auto submenu : submenus )
	{
		result.append( submenu.xml());
	}
	return result;
}

const Glib::RefPtr<Gio::ActionGroup> menu::get_actions()
{
	auto action_group = Gio::SimpleActionGroup::create();
	for ( auto submenu : submenus )
	{
		for ( auto item: submenu.menu_items)
		{
			action_group->add_action( item.title, item.lambda );
		}
	}
	return action_group;
}
