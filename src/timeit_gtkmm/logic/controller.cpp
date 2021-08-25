
#include <edit_task_dialog.h>
#include "controller.h"
#include <details_dialog.h>
#include <libtimeit/os_abstraction.h>
#include <iostream>
#include <libtimeit/utils.h>
#include <libtimeit/db/default_values.h>
#include <window_manager.h>
#include <main_window/main_window.h>
#include <edit_time.h>


using namespace gui;
using namespace libtimeit;
using namespace std;

widget_controller::widget_controller(
		window_manager&        op_gui_factory,
		Time_keeper&           op_time_keeper,
		database&              db,
		notification_manager&  op_notifier,
		image_cache&           images)
		:
		time_keeper_observer( op_time_keeper),
		event_observer( op_notifier ),
		windows( op_gui_factory),
		time_keeper( op_time_keeper),
		times( db),
		settings( db),
		db( db),
		notifier( op_notifier),
		images( images)


{
}

widget_controller::~widget_controller()
{
	windows.get_widget(MAIN_WINDOW)->detach(this);
}

void widget_controller::start()
{
	windows.get_status_icon().show();
	windows.get_status_icon().attach(this);
	if (!settings.get_bool("StartMinimized", DEFAULT_START_MINIMIZED))
	{
		shared_ptr<main_window> window = static_pointer_cast<main_window>(windows.get_widget(MAIN_WINDOW));
		action_observer* observer=this;
		window->attach(observer);
		window->show();
	}
	for( auto item: times.by_state( PAUSED))
	{
		show_idle_dialog( item.id );
	}

}

//LCOV_EXCL_START
void widget_controller::on_action_quit()
{
	windows.quit();
}
//LCOV_EXCL_STOP

void widget_controller::on_action_toggle_main_window()
{
	shared_ptr<main_window> window = static_pointer_cast<main_window>(windows.get_widget(MAIN_WINDOW));
	if (window->is_visible())
	{
		window->get_position(main_window_x, main_window_y);
		window->hide();
	}
	else
	{
		static bool first_time = true;
		if (!first_time)
		{
			window->move(main_window_x, main_window_y);
		}
		first_time = false;
		action_observer* observer = this;
		window->attach(observer);
		window->show();
	}
}
void widget_controller::on_show_main_window()
{
	widget_ptr window = windows.get_widget( MAIN_WINDOW);
	window->show();
}

void widget_controller::on_action_about()
{
	windows.get_widget(ABOUT_DIALOG)->show();
}
//LCOV_EXCL_START
void widget_controller::on_action_report_bug()
{
	show_url( "https://github.com/Hoglet/TimeIT/issues/new" );
}

void widget_controller::on_action_help()
{
	stringstream translated_help { };
	stringstream help_to_use { };
	translated_help << PACKAGE_DATA_DIR << "/doc/timeit/html/" << iso_639_language_string() << "/index.html";
	if (file_exists(string(translated_help.str())))
	{
		help_to_use << "file://" << translated_help.str();
	}
	else
	{
		stringstream default_help;
		default_help << PACKAGE_DATA_DIR << "/doc/timeit/html/C/index.html";
		if (file_exists(string(default_help.str())))
		{
			help_to_use << "file://" << default_help.str();
		}
	}
	show_url( help_to_use.str());
}
//LCOV_EXCL_STOP
void widget_controller::on_action_start_task()
{
	if(selected_task_id.has_value())
	{
		time_keeper.start(selected_task_id.value());
	}
}

void widget_controller::on_action_stop_task()
{
	if(selected_task_id.has_value())
	{
		time_keeper.stop(selected_task_id.value());
	}
}

void widget_controller::on_action_edit_task()
{
	//EditTaskDialog* dialog;

	//m_refXML->get_widget_derived("EditTaskDialog", dialog);
	if (selected_task_id.has_value())
	{
		widget_ptr edit_task = windows.get_widget( EDIT_TASK_DIALOG);
		dynamic_pointer_cast<edit_task_dialog>(edit_task)->set_task_id(selected_task_id.value());
		edit_task->show();
	}
}

void widget_controller::on_action_add_time()
{
	if (selected_task_id.has_value())
	{
		auto now = libtimeit::now();
		time_entry item( selected_task_id.value(), now, now);
		auto dialog = make_shared<gui::edit_time_dialog>( item, db);
		windows.manage_lifespan(dialog);
		dialog->show();
	}
}

void widget_controller::on_action_remove_task()
{
	//ENHANCEMENT Move code from main window (or?)
}

void widget_controller::on_idle_detected( const time_entry& te)
{
	on_idle_changed();

	times.update(te.with(PAUSED));
	show_idle_dialog(te.id);
}

void widget_controller::on_idle_changed()
{
	shared_ptr<main_window> window = dynamic_pointer_cast<main_window>(windows.get_widget(MAIN_WINDOW));
	window->on_running_tasks_changed();
}

void widget_controller::on_action_stop_timers()
{
	time_keeper.stop_all();
}

void widget_controller::on_action_task_selection_changed(optional<task_id> selected_task_id)
{
	this->selected_task_id = selected_task_id;
}

void widget_controller::on_action_add_task()
{
	widget_ptr add_task_dialog = windows.get_widget( ADD_TASK_DIALOG);
	dynamic_pointer_cast<edit_task_dialog>(add_task_dialog)->set_parent(selected_task_id);
	add_task_dialog->show();
}

void widget_controller::on_action_preferences()
{
	widget_ptr dialog = windows.get_widget( PREFERENCE_DIALOG);
	dialog->show();
}

void widget_controller::on_show_details_clicked( const task_id& id, time_t start_time, time_t stop_time)
{
	auto dialog = make_shared<details_dialog>(db, time_keeper, notifier, windows, images);
	if (dialog)
	{
		windows.manage_lifespan(dialog);
		dialog->set( id, start_time, stop_time);
		dialog->show();
	}
}

void widget_controller::on_time_entry_changed(const time_entry& /*te*/)
{
	auto currently_running = times.currently_running();
	if(currently_running != old_running)
	{
		on_running_changed();
	}
	old_running = currently_running;
}

//LCOV_EXCL_START
void widget_controller::on_running_changed()
{
	shared_ptr<main_window> window = dynamic_pointer_cast<main_window>(windows.get_widget(MAIN_WINDOW));
	window->on_running_tasks_changed();
}

void widget_controller::on_selection_changed(optional<task_id> /*task_id*/, time_t /*start*/, time_t /*stop*/)
{
}

void widget_controller::show_idle_dialog(const time_id& id)
{
	auto dialog = dynamic_pointer_cast<idle_dialog>(windows.get_widget(gui::IDLE_DIALOG));
	dialog->set_time_id(id);
	dialog->show();
}

//LCOV_EXCL_STOP
