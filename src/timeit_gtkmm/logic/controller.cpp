
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
		guiFactory(op_guiFactory),
		timeKeeper(op_timeKeeper),
		timeAccessor(database),
		settingsAccessor(database)

{
	timeKeeper.attach(this);
	ipc.attach(this);
}

Controller::~Controller()
{
	guiFactory.getWidget(MAIN_WINDOW)->detach(this);
	timeKeeper.detach(this);
}

void Controller::start()
{
	guiFactory.getStatusIcon().show();
	guiFactory.getStatusIcon().attach(this);
	if (!settingsAccessor.get_bool("StartMinimized", DEFAULT_START_MINIMIZED))
	{
		WidgetPtr mainWindow = guiFactory.getWidget(MAIN_WINDOW);
		mainWindow->attach(this);
		mainWindow->show();
	}
}

//LCOV_EXCL_START
void Controller::on_action_quit()
{
	guiFactory.quit();
}
//LCOV_EXCL_STOP

void Controller::on_action_toggleMainWindow()
{
	WidgetPtr mainWindow = guiFactory.getWidget(MAIN_WINDOW);
	if (mainWindow->is_visible())
	{
		mainWindow->get_position(mainWindow_x, mainWindow_y);
		mainWindow->hide();
	}
	else
	{
		static bool firstTime = true;
		if (!firstTime)
		{
			mainWindow->move(mainWindow_x, mainWindow_y);
		}
		firstTime = false;
		guiFactory.getWidget(MAIN_WINDOW)->attach(this);
		mainWindow->show();
	}
}
void Controller::on_show_main_window()
{
	WidgetPtr mainWindow = guiFactory.getWidget(MAIN_WINDOW);
	mainWindow->show();
}

void Controller::on_action_about()
{
	guiFactory.getWidget(ABOUT_DIALOG)->show();
}
//LCOV_EXCL_START
void Controller::on_action_report_bug()
{
	show_URL("https://github.com/Hoglet/TimeIT/issues/new");
}

void Controller::on_action_help()
{
	stringstream translatedHelp { };
	stringstream helpToUse { };
	translatedHelp << PACKAGE_DATA_DIR << "/doc/timeit/html/" << ISO_639_language_string() << "/index.html";
	if (file_exists(string(translatedHelp.str())))
	{
		helpToUse << "file://" << translatedHelp.str();
	}
	else
	{
		stringstream defaultHelp;
		defaultHelp << PACKAGE_DATA_DIR << "/doc/timeit/html/C/index.html";
		if (file_exists(string(defaultHelp.str())))
		{
			helpToUse << "file://" << defaultHelp.str();
		}
	}
	show_URL(helpToUse.str());
}
//LCOV_EXCL_STOP
void Controller::on_action_start_task()
{
	timeKeeper.start(selectedTaskID);
}

void Controller::on_action_stop_task()
{
	timeKeeper.stop(selectedTaskID);
}

void Controller::on_action_edit_task()
{
	//EditTaskDialog* dialog;

	//m_refXML->get_widget_derived("EditTaskDialog", dialog);
	if (selectedTaskID > 0)
	{
		WidgetPtr editTaskDialog = guiFactory.getWidget(EDIT_TASK_DIALOG);
		dynamic_pointer_cast<Edit_task_dialog>(editTaskDialog)->set_task_id(selectedTaskID);
		editTaskDialog->show();
	}
}

void Controller::on_action_add_time()
{
	if (selectedTaskID > 0)
	{
		guiFactory.getAddTime(selectedTaskID)->show();
	}
}

void Controller::on_action_remove_task()
{
	//ENHANCEMENT Move code from main window (or?)
}

void Controller::on_activity_resumed()
{
	if( ! timeKeeper.hasRunningTasks())
	{
		timeKeeper.enable(false);
		return;
	}
	bool quiet = settingsAccessor.get_bool("Quiet", DEFAULT_QUIET_MODE);
	if(quiet)
	{
		on_action_revertAndContinue();
	}
	else
	{
		idleDialog = dynamic_pointer_cast<IdleDialog>(guiFactory.getWidget(GUI::IDLE_DIALOG));
		idleDialog->attach(this);
		idleDialog->setIdleStartTime(idleStartTime);
		vector<int64_t> taskIDs = timeAccessor.currently_running();
		idleDialog->setActiveTaskList(taskIDs);
		idleDialog->show();
	}
	on_idleChanged();
}

void Controller::on_idle_detected()
{
	timeKeeper.enable(false);
	time_t now = time(0);
	idleStartTime = now - timeKeeper.time_idle();
	on_idleChanged();
}

void Controller::on_idleChanged()
{
	shared_ptr<MainWindow> mainWindow = dynamic_pointer_cast<MainWindow>(guiFactory.getWidget(MAIN_WINDOW));
	mainWindow->on_runningTasksChanged();
	shared_ptr<DetailsDialog> detailsDialog = dynamic_pointer_cast<DetailsDialog>(guiFactory.getWidget(DETAILS_DIALOG));
	detailsDialog->on_runningTasksChanged();
}

void Controller::on_action_revertAndContinue()
{
	timeKeeper.stop_all_and_continue();
	timeKeeper.enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}

}
void Controller::on_action_revertAndStop()
{
	timeKeeper.stop_all();
	timeKeeper.enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}
}
void Controller::on_action_continue()
{
	timeKeeper.enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}
}
void Controller::on_action_stopTimers()
{
	timeKeeper.stop_all();
}

void Controller::on_action_task_selection_changed(int selected_task_ID)
{
	this->selectedTaskID = selected_task_ID;
}

void Controller::on_action_add_task()
{
	WidgetPtr addTaskDialog = guiFactory.getWidget(ADD_TASK_DIALOG);
	dynamic_pointer_cast<Edit_task_dialog>(addTaskDialog)->set_parent(selectedTaskID);
	addTaskDialog->show();
}

void Controller::on_action_preferences()
{
	WidgetPtr preferenceDialog = guiFactory.getWidget(PREFERENCE_DIALOG);
	preferenceDialog->show();
}

void Controller::on_showDetailsClicked( int64_t taskId, time_t startTime, time_t stopTime)
{
	shared_ptr<DetailsDialog> detailsDialog = dynamic_pointer_cast<DetailsDialog>(
			guiFactory.getWidget(GUI::DETAILS_DIALOG));
	if (detailsDialog)
	{
		detailsDialog->set(taskId, startTime, stopTime);
		detailsDialog->show();
	}
}

//LCOV_EXCL_START
void Controller::on_running_changed()
{
	shared_ptr<MainWindow> mainWindow = dynamic_pointer_cast<MainWindow>(guiFactory.getWidget(MAIN_WINDOW));
	mainWindow->on_runningTasksChanged();
	shared_ptr<DetailsDialog> detailsDialog = dynamic_pointer_cast<DetailsDialog>(guiFactory.getWidget(DETAILS_DIALOG));
	detailsDialog->on_runningTasksChanged();
}
void Controller::on_selection_changed(int64_t, time_t, time_t)
{
}

//LCOV_EXCL_STOP
