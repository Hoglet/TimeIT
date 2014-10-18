/*
 * Controller.cpp
 *
 *  Created on: Mar 12, 2009
 *      Author: hoglet
 */

#include <IEditTaskDialog.h>
#include <IAddTaskDIalog.h>
#include "Controller.h"
#include <IDetailsDialog.h>
#include <OSAbstraction.h>
#include <iostream>
#include <Utils.h>
#include <DefaultValues.h>
#include <GUIFactory.h>
#include <libnotify/notify.h>

constexpr auto DUPLICATE_RESHOW_TIME = 600;

using namespace GUI;

Controller::Controller(std::shared_ptr<GUI::IGUIFactory>& op_guiFactory, std::shared_ptr<ITimeKeeper>& op_timeKeeper,
		std::shared_ptr<DB::IDatabase>& database) :
		guiFactory(op_guiFactory), timeKeeper(op_timeKeeper), taskAccessor(database->getExtendedTaskAccessor()), settingsAccessor(
				database->getSettingsAccessor())
{
	timeKeeper->attach(this);
}

Controller::~Controller()
{
	guiFactory->getWidget(MAIN_WINDOW)->detach(this);
	timeKeeper->detach(this);
}

void Controller::start()
{
	guiFactory->getStatusIcon().show();
	guiFactory->getStatusIcon().attach(this);
	if (!settingsAccessor->GetBoolByName("StartMinimized", DEFAULT_START_MINIMIZED))
	{
		WidgetPtr mainWindow = guiFactory->getWidget(MAIN_WINDOW);
		mainWindow->attach(this);
		mainWindow->show();
	}
}

//LCOV_EXCL_START
void Controller::on_action_quit()
{
	GUI::GUIFactory::quit();
}
//LCOV_EXCL_STOP

void Controller::on_action_toggleMainWindow()
{
	WidgetPtr mainWindow = guiFactory->getWidget(MAIN_WINDOW);
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
		guiFactory->getWidget(MAIN_WINDOW)->attach(this);
		mainWindow->show();
	}
}

void Controller::on_action_about()
{
	guiFactory->getWidget(ABOUT_DIALOG)->show();
}
//LCOV_EXCL_START
void Controller::on_action_report_bug()
{
	OSAbstraction::showURL("https://github.com/Hoglet/TimeIT/issues/new");
}

void Controller::on_action_help()
{
	std::stringstream translatedHelp
		{
		};
	std::stringstream helpToUse
		{
		};
	translatedHelp << PACKAGE_DATA_DIR << "/doc/timeit/html/" << Utils::get639LanguageString() << "/index.html";
	if (OSAbstraction::fileExists(std::string(translatedHelp.str())))
	{
		helpToUse << "file://" << translatedHelp.str();
	}
	else
	{
		std::stringstream defaultHelp;
		defaultHelp << PACKAGE_DATA_DIR << "/doc/timeit/html/C/index.html";
		if (OSAbstraction::fileExists(std::string(defaultHelp.str())))
		{
			helpToUse << "file://" << defaultHelp.str();
		}
	}
	OSAbstraction::showURL(helpToUse.str().c_str());
}
//LCOV_EXCL_STOP
void Controller::on_action_start_task()
{
	timeKeeper->StartTask(selectedTaskID);
}

void Controller::on_action_stop_task()
{
	timeKeeper->StopTask(selectedTaskID);
}

void Controller::on_action_edit_task()
{
	//EditTaskDialog* dialog;

	//m_refXML->get_widget_derived("EditTaskDialog", dialog);
	if (selectedTaskID)
	{
		WidgetPtr editTaskDialog = guiFactory->getWidget(EDIT_TASK_DIALOG);
		std::dynamic_pointer_cast<IEditTaskDialog>(editTaskDialog)->setTaskID(selectedTaskID);
		editTaskDialog->show();
	}
}

void Controller::on_action_add_time()
{
	if (selectedTaskID)
	{
		guiFactory->getAddTime(selectedTaskID)->show();
	}
}

void Controller::on_action_remove_task()
{
	//ENHANCEMENT Move code from main window (or?)
}

void Controller::on_idleDetected()
{
	timeKeeper->enable(false);
	time_t now = time(0);
	time_t idleStartTime = now - timeKeeper->timeIdle();
	bool quiet = settingsAccessor->GetBoolByName("Quiet", DEFAULT_QUIET_MODE);
	if (quiet)
	{
		on_action_revertAndContinue();
	}
	else
	{
		idleDialog = std::dynamic_pointer_cast<IIdleDialog>(guiFactory->getWidget(GUI::IDLE_DIALOG));
		idleDialog->attach(this);
		idleDialog->setIdleStartTime(idleStartTime);
		std::shared_ptr<std::vector<DB::ExtendedTask> > activeTasks = taskAccessor->getRunningTasks();
		idleDialog->setActiveTaskList(activeTasks);
		idleDialog->show();
	}
}

void Controller::on_action_revertAndContinue()
{
	timeKeeper->stopAllAndContinue();
	timeKeeper->enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}

}
void Controller::on_action_revertAndStop()
{
	timeKeeper->stopAll();
	timeKeeper->enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}
}
void Controller::on_action_continue()
{
	timeKeeper->enable(true);
	if (idleDialog != 0)
	{
		idleDialog->detach(this);
		idleDialog.reset();
	}
}
void Controller::on_action_stopTimers()
{
	timeKeeper->stopAll();
}

void Controller::on_action_task_selection_changed(int selectedTaskID)
{
	this->selectedTaskID = selectedTaskID;
}

void Controller::on_action_add_task()
{
	WidgetPtr addTaskDialog = guiFactory->getWidget(ADD_TASK_DIALOG);
	std::dynamic_pointer_cast<IAddTaskDialog>(addTaskDialog)->setParent(selectedTaskID);
	addTaskDialog->show();
}

void Controller::on_action_preferences()
{
	WidgetPtr preferenceDialog = guiFactory->getWidget(PREFERENCE_DIALOG);
	preferenceDialog->show();
}

void Controller::on_showDetailsClicked(ISummary* summary, int64_t taskId, time_t startTime, time_t stopTime)
{
	std::shared_ptr<IDetailsDialog> detailsDialog = std::dynamic_pointer_cast<IDetailsDialog>(
			guiFactory->getWidget(GUI::DETAILS_DIALOG));
	if (detailsDialog)
	{
		detailsDialog->set(taskId, startTime, stopTime);
		detailsDialog->show();
	}
}

//LCOV_EXCL_START
void Controller::on_runningChanged()
{
}
void Controller::on_selection_changed(int64_t id, time_t startTime, time_t stopTime)
{
}

//LCOV_EXCL_STOP
