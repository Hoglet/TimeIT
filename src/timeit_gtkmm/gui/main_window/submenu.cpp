

#include <fmt/format.h>
#include <gtkmm/menu.h>
#include <gtkmm/builder.h>
#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
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
submenu& submenu::add_item( )
{
	menu_items.push_back(menu_item());
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

std::unique_ptr<Gtk::Menu> submenu::create( )
{
	auto xml = fmt::format(R"(
		<interface>
			<menu id='{}'>
				<section>
					{}
				</section>
			</menu>
		</interface>)"	,
		"menu_id", items_xml());

	Glib::RefPtr<Gtk::Builder> m_refBuilder = Gtk::Builder::create();
	m_refBuilder->add_from_string(xml);
	Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object("menu_id");
	Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);

	return std::make_unique<Gtk::Menu>(gmenu);
}

const Glib::RefPtr<Gio::ActionGroup> submenu::get_actions( )
{
	auto action_group = Gio::SimpleActionGroup::create();
	for ( auto item: menu_items)
	{
		action_group->add_action( item.title, item.lambda );
	}
	return action_group;
}
