#include "main_window.h"
#include "task_list.h"
#include <gtkmm.h>
#include <iostream>
#include <libtimeit/utils.h>
#include <libtimeit/os_abstraction.h>
#include <glibmm/i18n.h>
#include "summary.h"
#include "menu.h"
#include "submenu.h"
#include <libtimeit/db/default_values.h>
#include <glibmm.h>

using namespace Gtk;
using namespace libtimeit;

namespace gui
{

main_window::~main_window()
{
	empty_containers();
	save_size();
	detach_from_all(this);
}

static const int DEFAULT_WINDOW_WIDTH = 550;
static const int DEFAULT_WINDOW_HEIGHT = 550;

main_window::main_window(
		database&             db,
		time_manager&         op_time_keeper,
		notification_manager& notifier,
		window_manager&       gui_factory,
		image_cache&          images,
		controller_interface&           op_controller)
		:
		task_list( db, op_time_keeper, notifier, images),
		day_summary(db, notifier),
		week_summary(db, notifier),
		month_summary(db, notifier),
		year_summary(db, notifier),
		label_day(
				_("Day")),
		label_week(_("Week")),
		label_month(_("Month")),
		label_year(_("Year")),
		details_view(db, notifier, gui_factory),
		times(db),
		tasks(db),
		settings(db),
		controller( op_controller )
{
	create_layout();
	relate_widgets();
	attach_to_all(this);
	show_all_children();
	on_running_tasks_changed();

	width = (int) settings.get_int("main_window_width", DEFAULT_WINDOW_WIDTH);
	height = (int) settings.get_int("main_window_height", DEFAULT_WINDOW_HEIGHT);
	set_default_size(width, height);

	signal_check_resize().connect( [&]{ this->resized();}   );
	signal_hide().connect(         [&]{ this->save_size();} );
}

void main_window::resized()
{
	auto w = get_width();
	auto h = get_height();
	if(w>1)
	{
		width = w;
	}
	if(h>1)
	{
		height = h;
	}
}

void main_window::save_size()
{
	settings.set_int("main_window_width", width);
	settings.set_int("main_window_height", height);
	settings.set_int("main_win_v_paned_position", v_paned.get_position());
	settings.set_int("main_win_h_paned_position", h_paned.get_position());
}

calendar_widget &main_window::get_calendar()
{
	return calendar;
}

void main_window::attach(action_observer* observer)
{
	attach_to_all(observer);
}

void main_window::attach_to_all(action_observer* observer)
{
	detach(observer); //To avoid duplicates
	task_list.attach(observer);
	toolbar.attach(observer);
	auto* s_observer = dynamic_cast<summary_observer*>(observer);
	if (s_observer != nullptr)
	{
		for (auto *item : summaries)
		{
			item->attach(s_observer);
		}
	}
}
void main_window::detach(action_observer* observer)
{
	detach_from_all( observer );
}

void main_window::detach_from_all(action_observer* observer)
{
	task_list.detach(observer);
	toolbar.detach(observer);
	auto* s_observer = dynamic_cast<summary_observer*>(observer);
	if (s_observer != nullptr)
	{
		for (auto *item : summaries)
		{
			item->detach(s_observer);
		}
	}
}

void main_window::relate_widgets()
{
	//Give widgets references to other widgets they need to communicate with
	summaries.push_back(&day_summary);
	summaries.push_back(&week_summary);
	summaries.push_back(&month_summary);
	summaries.push_back(&year_summary);

	for (auto *widget : summaries)
	{
		widget->set_references(calendar);
		widget->attach(&details_view);
	}
}

void main_window::create_layout()
{
	//First setting parameters on widgets
	task_list_container.set_size_request(290, 270);
	task_list_container.set_shadow_type(Gtk::SHADOW_NONE);
	task_list_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	calendar.set_display_options( Gtk::CALENDAR_SHOW_HEADING | Gtk::CALENDAR_SHOW_DAY_NAMES | Gtk::CALENDAR_SHOW_WEEK_NUMBERS);
	calendar.set_size_request(530,260);
	day_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	day_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	week_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	week_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	month_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	month_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	year_summary_container.set_shadow_type(Gtk::SHADOW_NONE);
	year_summary_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	summary_tabs.set_size_request(290, 150);

	menu_bar = create_menubar();
	update_menus( false );

	//Then the actual layout
	details_view.set_size_request(400, 220);
	do_layout();
	details_window.add(details_view);
	details_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	h_paned.set_position((int) settings.get_int("main_win_h_paned_position", get_width()  * 4 / 10));
	v_paned.set_position((int) settings.get_int("main_win_v_paned_position", get_height() * 3 /10));
	v_paned.pack2(details_window);

	show_all_children();
	grab_focus();
}

using namespace Glib::Container_Helpers;

void main_window::remove_children(Container &container)
{
	Glib::ListHandle<Gtk::Widget*> listh = container.get_children();
	for (auto iter = listh.begin(); iter != listh.end(); ++iter)
	{
		Gtk::Widget* widget = (*iter);
		container.remove(*widget);
	}
}

void main_window::empty_containers()
{
	remove_children(*this);
	remove_children(h_paned);
	remove_children(main_v_box);
	remove_children(task_list_container);
	remove_children(secondary_v_box);
	remove_children(day_summary_container);
	remove_children(week_summary_container);
	remove_children(month_summary_container);
	remove_children(year_summary_container);
	remove_children(summary_tabs);
}

void main_window::on_settings_changed(string name)
{
	if (name == "CompactLayout")
	{
		do_layout();
	}
}

void main_window::do_layout()
{
	if (settings.get_bool("CompactLayout", DEFAULT_COMPACT_LAYOUT))
	{
		empty_containers();
		default_layout();
	}
	else
	{
		empty_containers();
		classic_layout();
	}
}

void main_window::default_layout()
{
	add(v_paned);
	v_paned.pack1(h_paned, Gtk::FILL);
	{
		h_paned.pack1(main_v_box, Gtk::FILL);
		{

			main_v_box.pack_start( *menu_bar, Gtk::PACK_SHRINK, 0);
			main_v_box.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
			main_v_box.pack_start(task_list_container); // Gtk::AttachOptions(0));

			{
				task_list_container.add(task_list);
			}
		}
		h_paned.pack2(secondary_v_box, Gtk::EXPAND);
		{
			secondary_v_box.pack_start(calendar, Gtk::PACK_SHRINK, 0);
			secondary_v_box.pack_start(summary_tabs);
			{
				day_summary_container.add(day_summary);
				week_summary_container.add(week_summary);
				month_summary_container.add(month_summary);
				year_summary_container.add(year_summary);
				summary_tabs.append_page(day_summary_container, label_day);
				summary_tabs.append_page(week_summary_container, label_week);
				summary_tabs.append_page(month_summary_container, label_month);
				summary_tabs.append_page(year_summary_container, label_year);

			}
		}

	}

	//mainVBox.pack_start(statusbar, Gtk::PACK_SHRINK, 0);

}

void main_window::classic_layout()
{
	add(v_paned);
	v_paned.pack1(main_v_box, Gtk::FILL);
	{
		main_v_box.pack_start(*menu_bar, Gtk::PACK_SHRINK, 0);
		main_v_box.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
		main_v_box.pack_start(h_paned);
		{
			h_paned.pack1(task_list_container, Gtk::FILL); // Gtk::AttachOptions(0));

			{
				task_list_container.add(task_list);
			}
			h_paned.pack2(secondary_v_box, Gtk::EXPAND);
			{
				secondary_v_box.pack_start(calendar, Gtk::PACK_SHRINK, 0);
				secondary_v_box.pack_start(summary_tabs);
				{
					day_summary_container.add(day_summary);
					week_summary_container.add(week_summary);
					month_summary_container.add(month_summary);
					year_summary_container.add(year_summary);
					summary_tabs.append_page(day_summary_container, label_day);
					summary_tabs.append_page(week_summary_container, label_week);
					summary_tabs.append_page(month_summary_container, label_month);
					summary_tabs.append_page(year_summary_container, label_year);
				}
			}

		}
		main_v_box.pack_start(statusbar, Gtk::PACK_SHRINK, 0);
	}
}

// also displays whether idle
void main_window::on_running_tasks_changed()
{
	auto running_tasks = times.currently_running();
	if (running_tasks.empty())
	{
		set_title("TimeIT");
	}
	else
	{
		set_title("TimeIT ⌚");
	}
	task_list.on_complete_update();
}

void main_window::on_show()
{
	Gtk::Window::on_show();
	Gtk::Window::deiconify();
	set_calendar();
}

static const int YEAR_ZERO = 1900;

void main_window::set_calendar()
{
	auto now = system_clock::now();
	struct tm time_info = localtime( now );
	auto month = (guint) time_info.tm_mon;
	auto year = (guint) time_info.tm_year + YEAR_ZERO;
	auto day = (guint) time_info.tm_mday;
	calendar.select_month(month, year);
	calendar.select_day(day);
}

void main_window::on_action_task_selection_changed(optional<task_id> selected_task_id)
{
	toolbar.set_task_is_selected(selected_task_id.has_value());
	this->update_menus(selected_task_id.has_value());
}

void main_window::on_action_remove_task()
{
	auto selected_task_id = task_list.selected_id();

	if( selected_task_id.has_value() == false )
	{
		return;
	}

	//Confirm dialog when removing task, headline!
	const char* headline = _("Are you sure you want to delete this task?");
	Gtk::MessageDialog dialog(*this, headline, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL);
	//Line after "Are you sure you want to delete this task?" inside the dialog
	dialog.set_secondary_text(_("There is no undelete function"));
	dialog.set_deletable(false);

	int result = dialog.run();

	//Handle the response:
	switch (result)
	{
		case (Gtk::RESPONSE_OK):
			//Remove task
			tasks.remove(selected_task_id.value());
			break;
		case (Gtk::RESPONSE_CANCEL):
			break;
		default:
			cerr << "Unexpected button clicked." << std::endl;
			break;
	}
}

unique_ptr<MenuBar> main_window::create_menubar( )
{
	submenu file_menu( _( "_File" ));
	file_menu
			.add_item( _( "Settings" ), "", [ & ] { this->controller.on_settings( ); } )
			.add_item( _( "Quit" ), "<Primary>q", [ & ] { this->controller.on_quit( ); } );

	submenu task_menu( _( "_Task" ));
	task_menu
			.add_item( _( "_Start" ), "", [ & ] { this->controller.on_start( ); } )
			.add_item( _( "Stop" ), "", [ & ] { this->controller.on_stop( ); } )
			.add_item( _( "Stop all" ), "", [ & ] { this->controller.on_stop_all( ); } )
			.add_item( _( "Edit" ), "", [ & ] { this->controller.on_edit_task( ); } )
			.add_item( )
			.add_item( _( "Add time" ), "", [ & ] { this->controller.on_add_time( ); } )
			.add_item( )
			.add_item( _( "Add task" ), "", [ & ] { this->controller.on_add_task( ); } )
			.add_item( _( "Remove" ), "", [ & ] { this->on_action_remove_task( ); } );

	submenu help_menu( _( "_Help" ));
	help_menu
			.add_item( _( "About" ), "", [ & ] { this->controller.on_about( ); } )
			.add_item( _( "Report bug" ), "", [ & ] { this->controller.on_report_bug( ); } )
			.add_item( _( "Help" ), "", [ & ] { this->controller.on_help( ); } );

	task_sensitive_menus = { _( "_Start" ),_( "Stop" ),_( "Stop all" ),_( "Edit" ), _( "Add time" ) };

	menu main_menu;
	main_menu
			.add( file_menu )
			.add( task_menu )
			.add( help_menu );

	return main_menu.get_menu_bar( );
}

list<MenuItem*> get_all_menuitems(vector<Widget*> widgets)
{
	list<MenuItem*> result;
	for ( auto widget: widgets )
	{
		auto m = dynamic_cast<MenuItem*>(widget);
		if ( m != nullptr )
		{
			result.push_back(m);
			if( m->has_submenu() )
			{
				result.splice( result.end(), get_all_menuitems(m->get_submenu()->get_children()) );
			}
		}
	}
	return result;
}



void main_window::update_menus( bool is_selected )
{
	for( auto menu_item: get_all_menuitems(menu_bar->get_children()))
	{
		string label = menu_item->get_label();
		for ( auto item: task_sensitive_menus )
		{
			if ( item == label )
			{
				menu_item->set_sensitive( is_selected );
			}
		}
	}
}

}
