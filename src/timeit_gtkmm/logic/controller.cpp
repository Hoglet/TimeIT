
#include <edit_task_dialog.h>
#include "controller.h"
#include <details_dialog.h>
#include <libtimeit/OS_abstraction.h>
#include <iostream>
#include <libtimeit/utils.h>
#include <libtimeit/db/default_values.h>
#include <gui_factory.h>
#include <main_window/main_window.h>
#include <edit_time.h>


using namespace gui;
using namespace libtimeit;
using namespace std;

Controller::Controller(
		Window_manager  &op_guiFactory,
		Time_keeper &op_timeKeeper,
		Database    &database_,
		IpcServer   &ipc,
		Notifier    &notifier)
		:
		Time_keeper_observer(op_timeKeeper),
		Event_observer( notifier ),
		gui_factory(op_guiFactory),
		time_keeper(op_timeKeeper),
		time_accessor(database_),
		settings_accessor(database_),
		database(database_)

{
	ipc.attach(this);
}

Controller::~Controller()
{
	gui_factory.get_widget(MAIN_WINDOW)->detach(this);
}

void Controller::start()
{
	gui_factory.get_status_icon().show();
	gui_factory.get_status_icon().attach(this);
	if (!settings_accessor.get_bool("StartMinimized", DEFAULT_START_MINIMIZED))
	{
		WidgetPtr main_window = gui_factory.get_widget(MAIN_WINDOW);
		main_window->attach(this);
		main_window->show();
	}
	for( auto time_entry: time_accessor.by_state(PAUSED))
	{
		show_idle_dialog(time_entry.id);
	}

}

//LCOV_EXCL_START
void Controller::on_action_quit()
{
	gui_factory.quit();
}
//LCOV_EXCL_STOP

void Controller::on_action_toggle_main_window()
{
	WidgetPtr main_window = gui_factory.get_widget(MAIN_WINDOW);
	if (main_window->is_visible())
	{
		main_window->get_position(main_window_x, main_window_y);
		main_window->hide();
	}
	else
	{
		static bool first_time = true;
		if (!first_time)
		{
			main_window->move(main_window_x, main_window_y);
		}
		first_time = false;
		gui_factory.get_widget(MAIN_WINDOW)->attach(this);
		main_window->show();
	}
}
void Controller::on_show_main_window()
{
	WidgetPtr main_window = gui_factory.get_widget(MAIN_WINDOW);
	main_window->show();
}

void Controller::on_action_about()
{
	gui_factory.get_widget(ABOUT_DIALOG)->show();
}
//LCOV_EXCL_START
void Controller::on_action_report_bug()
{
	show_URL("https://github.com/Hoglet/TimeIT/issues/new");
}

void Controller::on_action_help()
{
	stringstream translated_help { };
	stringstream help_to_use { };
	translated_help << PACKAGE_DATA_DIR << "/doc/timeit/html/" << ISO_639_language_string() << "/index.html";
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
	show_URL(help_to_use.str());
}
//LCOV_EXCL_STOP
void Controller::on_action_start_task()
{
	time_keeper.start(selected_task_id);
}

void Controller::on_action_stop_task()
{
	time_keeper.stop(selected_task_id);
}

void Controller::on_action_edit_task()
{
	//EditTaskDialog* dialog;

	//m_refXML->get_widget_derived("EditTaskDialog", dialog);
	if (selected_task_id > 0)
	{
		WidgetPtr edit_task_dialog = gui_factory.get_widget(EDIT_TASK_DIALOG);
		dynamic_pointer_cast<Edit_task_dialog>(edit_task_dialog)->set_task_id(selected_task_id);
		edit_task_dialog->show();
	}
}

void Controller::on_action_add_time()
{
	if (selected_task_id > 0)
	{
		auto now = libtimeit::now();
		Time_entry time_entry(selected_task_id, now, now);
		auto dialog = make_shared<gui::Edit_time>(time_entry, database);
		gui_factory.manage_lifespan(dialog);
		dialog->show();
	}
}

void Controller::on_action_remove_task()
{
	//ENHANCEMENT Move code from main window (or?)
}

void Controller::on_idle_detected(Time_id id)
{
	on_idle_changed();

	auto time_entry = time_accessor.by_ID(id);
	if(time_entry.has_value())
	{
		time_accessor.update(time_entry->with(PAUSED));
		show_idle_dialog(id);
	}
}

void Controller::on_idle_changed()
{
	shared_ptr<MainWindow> main_window = dynamic_pointer_cast<MainWindow>(gui_factory.get_widget(MAIN_WINDOW));
	main_window->on_running_tasks_changed();
	shared_ptr<Details_dialog> details_dialog = dynamic_pointer_cast<Details_dialog>(
			gui_factory.get_widget(DETAILS_DIALOG));
	details_dialog->on_running_tasks_changed();
}

void Controller::on_action_stop_timers()
{
	time_keeper.stop_all();
}

void Controller::on_action_task_selection_changed(Task_id selected_task_ID)
{
	this->selected_task_id = selected_task_ID;
}

void Controller::on_action_add_task()
{
	WidgetPtr add_task_dialog = gui_factory.get_widget(ADD_TASK_DIALOG);
	dynamic_pointer_cast<Edit_task_dialog>(add_task_dialog)->set_parent(selected_task_id);
	add_task_dialog->show();
}

void Controller::on_action_preferences()
{
	WidgetPtr preference_dialog = gui_factory.get_widget(PREFERENCE_DIALOG);
	preference_dialog->show();
}

void Controller::on_show_details_clicked(int64_t taskId, time_t startTime, time_t stopTime)
{
	shared_ptr<Details_dialog> details_dialog = dynamic_pointer_cast<Details_dialog>(
			gui_factory.get_widget(gui::DETAILS_DIALOG));
	if (details_dialog)
	{
		details_dialog->set(taskId, startTime, stopTime);
		details_dialog->show();
	}
}

void Controller::on_time_entry_changed(Time_id /*id*/)
{
	auto currently_running = time_accessor.currently_running();
	if(currently_running != old_running)
	{
		on_running_changed();
	}
	old_running = currently_running;
}

//LCOV_EXCL_START
void Controller::on_running_changed()
{
	shared_ptr<MainWindow> main_window = dynamic_pointer_cast<MainWindow>(gui_factory.get_widget(MAIN_WINDOW));
	main_window->on_running_tasks_changed();
	shared_ptr<Details_dialog> details_dialog = dynamic_pointer_cast<Details_dialog>(
			gui_factory.get_widget(DETAILS_DIALOG));
	details_dialog->on_running_tasks_changed();
}
void Controller::on_selection_changed(int64_t /*task_id*/, time_t /*start*/, time_t /*stop*/)
{
}

void Controller::show_idle_dialog(Time_id id)
{
	auto idle_dialog = dynamic_pointer_cast<Idle_dialog>(gui_factory.get_widget(gui::IDLE_DIALOG));
	idle_dialog->set_time_id(id);
	idle_dialog->show();
}

//LCOV_EXCL_STOP
