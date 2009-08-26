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
//TODO Remove references to gtk
#include <gtkmm.h>

using namespace GUI;

Controller::Controller(boost::shared_ptr<GUI::IGUIFactory>& guiFactory, boost::shared_ptr<ITimeKeeper>& timeKeeper)
{
	selectedTaskID = -1;
	this->timeKeeper = timeKeeper;
	this->guiFactory = guiFactory;
	this->timeKeeper->attach(this);
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
}

void Controller::on_action_quit()
{
	//TODO move to GUIFactory or something better
	Gtk::Main::quit();
}

void Controller::on_action_toggleMainWindow()
{
	static bool firstTime = true;
	boost::shared_ptr<Gtk::Window> mainWindow(boost::dynamic_pointer_cast<Gtk::Window>(guiFactory->getWidget(MAIN_WINDOW)));
	if (mainWindow->is_visible())
	{
		mainWindow->get_position(mainWindow_x, mainWindow_y);
		mainWindow->hide();
	}
	else
	{
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

void Controller::on_action_help()
{
	OSAbstraction::showURL("http://devcorner.solit.se/wiki/TimeIT%20help%20page");
}
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
		boost::dynamic_pointer_cast<IEditTaskDialog>(editTaskDialog)->setTaskID(selectedTaskID);
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
	//TODO Move code from main window (or?)
}

void Controller::on_idleDetected()
{
	timeKeeper->enable(false);
	time_t now = time(0);
	time_t idleStartTime = now - timeKeeper->timeIdle();
	idleDialog = boost::dynamic_pointer_cast<IIdleDialog>(guiFactory->getWidget(GUI::IDLE_DIALOG));
	idleDialog->attach(this);
	idleDialog->setIdleStartTime(idleStartTime);
	idleDialog->show();
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

void Controller::on_action_task_selection_changed(int selectedTaskID)
{
	this->selectedTaskID = selectedTaskID;
}

void Controller::on_action_add_task()
{
	WidgetPtr addTaskDialog = guiFactory->getWidget(ADD_TASK_DIALOG);
	boost::dynamic_pointer_cast<IAddTaskDialog>(addTaskDialog)->setParent(selectedTaskID);
	addTaskDialog->show();
}

void Controller::on_action_preferences()
{
	WidgetPtr preferenceDialog = guiFactory->getWidget(PREFERENCE_DIALOG);
	preferenceDialog->show();
}



void Controller::on_showDetailsClicked(ISummary* summary,int64_t taskId, time_t startTime, time_t stopTime)
{
	boost::shared_ptr<IDetailsDialog> detailsDialog = boost::dynamic_pointer_cast<IDetailsDialog>(guiFactory->getWidget(GUI::DETAILS_DIALOG));
	if(detailsDialog)
	{
		detailsDialog->observe(summary);
		detailsDialog->set(taskId,startTime,stopTime);
		detailsDialog->show();
	}
}
