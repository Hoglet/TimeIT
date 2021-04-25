
#include <edit_task_dialog.h>
#include "controller.h"
#include <details_dialog.h>
#include <libtimeit/OS_abstraction.h>
#include <iostream>
#include <libtimeit/utils.h>
#include <libtimeit/db/default_values.h>
#include <guid_factory.h>
#include <main_window/main_window.h>


using namespace GUI;
using namespace libtimeit;
using namespace std;

Controller::Controller(
		IGUIFactory &op_guiFactory,
		Time_keeper &op_timeKeeper,
		Database    &database,
		IpcServer   &ipc,
		Notifier    &notifier)
		:
		Event_observer( notifier ),
		gui_factory(op_guiFactory),
		time_keeper(op_timeKeeper),
		time_accessor(database),
		settings_accessor(database)

{
	time_keeper.attach(this);
	ipc.attach(this);
}

Controller::~Controller()
{
	gui_factory.getWidget(MAIN_WINDOW)->detach(this);
	time_keeper.detach(this);
}

void Controller::start()
{
	gui_factory.getStatusIcon().show();
	gui_factory.getStatusIcon().attach(this);
	if (!settings_accessor.get_bool("StartMinimized", DEFAULT_START_MINIMIZED))
	{
		WidgetPtr main_window = gui_factory.getWidget(MAIN_WINDOW);
		main_window->attach(this);
		main_window->show();
	}
}

//LCOV_EXCL_START
void Controller::on_action_quit()
{
	gui_factory.quit();
}
//LCOV_EXCL_STOP

void Controller::on_action_toggleMainWindow()
{
	WidgetPtr main_window = gui_factory.getWidget(MAIN_WINDOW);
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
		gui_factory.getWidget(MAIN_WINDOW)->attach(this);
		main_window->show();
	}
}
void Controller::on_show_main_window()
{
	WidgetPtr main_window = gui_factory.getWidget(MAIN_WINDOW);
	main_window->show();
}

void Controller::on_action_about()
{
	gui_factory.getWidget(ABOUT_DIALOG)->show();
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
		WidgetPtr edit_task_dialog = gui_factory.getWidget(EDIT_TASK_DIALOG);
		dynamic_pointer_cast<Edit_task_dialog>(edit_task_dialog)->set_task_id(selected_task_id);
		edit_task_dialog->show();
	}
}

void Controller::on_action_add_time()
{
	if (selected_task_id > 0)
	{
		gui_factory.getAddTime(selected_task_id)->show();
	}
}

void Controller::on_action_remove_task()
{
	//ENHANCEMENT Move code from main window (or?)
}

void Controller::on_activity_resumed()
{
	if( ! time_keeper.hasRunningTasks())
	{
		auto i=4;
		//time_keeper.enable(false);
		return;
	}
	bool quiet = settings_accessor.get_bool("Quiet", DEFAULT_QUIET_MODE);
	if(quiet)
	{
		on_action_revertAndContinue();
	}
	else
	{
		idle_dialog = dynamic_pointer_cast<IdleDialog>(gui_factory.getWidget(GUI::IDLE_DIALOG));
		idle_dialog->attach(this);
		idle_dialog->setIdleStartTime(idle_start_time);
		vector<int64_t> task_ids = time_accessor.currently_running();
		idle_dialog->setActiveTaskList(task_ids);
		idle_dialog->show();
	}
	on_idleChanged();
}

void Controller::on_idle_detected( Time_ID id)
{
	int i=4;
	//time_keeper.enable(false);
	time_t now = time(nullptr);
	idle_start_time = now - time_keeper.time_idle();
	on_idleChanged();
}

void Controller::on_idleChanged()
{
	shared_ptr<MainWindow> main_window = dynamic_pointer_cast<MainWindow>(gui_factory.getWidget(MAIN_WINDOW));
	main_window->on_runningTasksChanged();
	shared_ptr<DetailsDialog> details_dialog = dynamic_pointer_cast<DetailsDialog>(gui_factory.getWidget(DETAILS_DIALOG));
	details_dialog->on_runningTasksChanged();
}

void Controller::on_action_revertAndContinue()
{
	time_keeper.stop_all_and_continue();
	//time_keeper.enable(true);
	if (idle_dialog != nullptr)
	{
		idle_dialog->detach(this);
		idle_dialog.reset();
	}

}
void Controller::on_action_revertAndStop()
{
	time_keeper.stop_all();
	//time_keeper.enable(true);
	if (idle_dialog != nullptr)
	{
		idle_dialog->detach(this);
		idle_dialog.reset();
	}
}
void Controller::on_action_continue()
{
	//time_keeper.enable(true);
	if (idle_dialog != nullptr)
	{
		idle_dialog->detach(this);
		idle_dialog.reset();
	}
}
void Controller::on_action_stopTimers()
{
	time_keeper.stop_all();
}

void Controller::on_action_task_selection_changed(int selected_task_ID)
{
	this->selected_task_id = selected_task_ID;
}

void Controller::on_action_add_task()
{
	 WidgetPtr add_task_dialog = gui_factory.getWidget(ADD_TASK_DIALOG);
	dynamic_pointer_cast<Edit_task_dialog>(add_task_dialog)->set_parent(selected_task_id);
	add_task_dialog->show();
}

void Controller::on_action_preferences()
{
	WidgetPtr preference_dialog = gui_factory.getWidget(PREFERENCE_DIALOG);
	preference_dialog->show();
}

void Controller::on_show_details_clicked(int64_t taskId, time_t startTime, time_t stopTime)
{
	shared_ptr<DetailsDialog> details_dialog = dynamic_pointer_cast<DetailsDialog>(
			gui_factory.getWidget(GUI::DETAILS_DIALOG));
	if (details_dialog)
	{
		details_dialog->set(taskId, startTime, stopTime);
		details_dialog->show();
	}
}

//LCOV_EXCL_START
void Controller::on_running_changed()
{
	shared_ptr<MainWindow> main_window = dynamic_pointer_cast<MainWindow>(gui_factory.getWidget(MAIN_WINDOW));
	main_window->on_runningTasksChanged();
	shared_ptr<DetailsDialog> details_dialog = dynamic_pointer_cast<DetailsDialog>(gui_factory.getWidget(DETAILS_DIALOG));
	details_dialog->on_runningTasksChanged();
}
void Controller::on_selection_changed(int64_t /*task_id*/, time_t /*start*/, time_t /*stop*/)
{
}

//LCOV_EXCL_STOP
