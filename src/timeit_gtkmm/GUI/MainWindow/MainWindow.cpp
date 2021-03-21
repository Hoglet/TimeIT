/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * TimeIT
 * Copyright (C) Kent Asplund 2008 <hoglet@solit.se>
 *
 * TimeIT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TimeIT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "TaskList.h"
#include <gtkmm.h>
#include "EditTaskDialog.h"
#include <iostream>
#include <libtimeit/Utils.h>
#include <libtimeit/OSAbstraction.h>
#include <vector>
#include <glibmm/i18n.h>
#include "Summary.h"
#include "DetailsDialog.h"
#include <libtimeit/db/DefaultValues.h>
#include <glibmm.h>

using namespace Gtk;
using namespace libtimeit;

namespace GUI
{

MainWindow::~MainWindow()
{
	detach(this);
	settingsAccessor->detach(this);
}

MainWindow::MainWindow(
		IDatabase &database,
		ITimeKeeper &timeKeeper)
		:
		taskList(database, timeKeeper),
		daySummary(database),
		weekSummary(database),
		monthSummary(database),
		yearSummary(database),
		labelDay(
				_("Day")),
				labelWeek(_("Week")),
				labelMonth(_("Month")),
				labelYear(_("Year")),
				timeAccessor(database.getTimeAccessor()),
				taskAccessor(database.getExtendedTaskAccessor()),
				settingsAccessor(database.getSettingsAccessor()
				)
{
	settingsAccessor->attach(this);
	createLayout();
	relateWidgets();
	attach(this);
	show_all_children();
	on_runningTasksChanged();
}

ICalendar& MainWindow::getCalendar()
{
	return calendar;
}

void MainWindow::attach(SummaryObserver *observer)
{
	vector<Summary*>::iterator iter;

	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->attach(observer);
	}
}
void MainWindow::detach(SummaryObserver *observer)
{
	vector<Summary*>::iterator iter;

	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->detach(observer);
	}
}

void MainWindow::attach(IActionObserver *observer)
{
	detach(observer); //To avoid duplicates
	taskList.attach(observer);
	toolbar.attach(observer);
	menubar.attach(observer);
	SummaryObserver *sObserver = dynamic_cast<SummaryObserver*>(observer);
	if (sObserver)
	{
		attach(sObserver);
	}
}

void MainWindow::detach(IActionObserver *observer)
{
	taskList.detach(observer);
	toolbar.detach(observer);
	menubar.detach(observer);
	SummaryObserver *sObserver = dynamic_cast<SummaryObserver*>(observer);
	if (sObserver)
	{
		detach(sObserver);
	}
}

void MainWindow::relateWidgets()
{
	//Give widgets references to other widgets they need too communicate with
	summaries.push_back(&daySummary);
	summaries.push_back(&weekSummary);
	summaries.push_back(&monthSummary);
	summaries.push_back(&yearSummary);

	vector<Summary*>::iterator iter;
	for (iter = summaries.begin(); iter != summaries.end(); ++iter)
	{
		Summary *summary = *iter;
		summary->setReferences(calendar);
	}
}

void MainWindow::createLayout()
{
	//First setting parameters on widgets
	TaskListContainer.set_size_request(270, 230);
	TaskListContainer.set_flags(Gtk::CAN_FOCUS);
	TaskListContainer.set_shadow_type(Gtk::SHADOW_NONE);
	TaskListContainer.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	calendar.set_flags(Gtk::CAN_FOCUS);
	calendar.display_options(
			Gtk::CALENDAR_SHOW_HEADING | Gtk::CALENDAR_SHOW_DAY_NAMES | Gtk::CALENDAR_SHOW_WEEK_NUMBERS);
	DaySummaryContainer.set_flags(Gtk::CAN_FOCUS);
	DaySummaryContainer.set_shadow_type(Gtk::SHADOW_NONE);
	DaySummaryContainer.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	WeekSummaryContainer.set_flags(Gtk::CAN_FOCUS);
	WeekSummaryContainer.set_shadow_type(Gtk::SHADOW_NONE);
	WeekSummaryContainer.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	MonthSummaryContainer.set_flags(Gtk::CAN_FOCUS);
	MonthSummaryContainer.set_shadow_type(Gtk::SHADOW_NONE);
	MonthSummaryContainer.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	YearSummaryContainer.set_flags(Gtk::CAN_FOCUS);
	YearSummaryContainer.set_shadow_type(Gtk::SHADOW_NONE);
	YearSummaryContainer.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	hPaned.set_flags(Gtk::CAN_FOCUS);
	summaryTabs.set_flags(Gtk::CAN_FOCUS);
	summaryTabs.set_size_request(250, 150);
	//Then the actual layout
	doLayout();

	show_all_children();
	grab_focus();
}

using namespace Glib::Container_Helpers;
void MainWindow::removeChildren(Container &container)
{
	Glib::ListHandle<Gtk::Widget*> listh = container.get_children();
	Glib::ListHandle<Gtk::Widget*>::iterator iter(listh.begin());
	for (iter = listh.begin(); iter != listh.end(); ++iter)
	{
		Gtk::Widget *widget = (*iter);
		container.remove(*widget);
	}
}

void MainWindow::emptyContainers()
{
	removeChildren(*this);
	removeChildren(hPaned);
	removeChildren(mainVBox);
	removeChildren(TaskListContainer);
	removeChildren(secondaryVBox);
	removeChildren(DaySummaryContainer);
	removeChildren(WeekSummaryContainer);
	removeChildren(MonthSummaryContainer);
	removeChildren(YearSummaryContainer);
	removeChildren(summaryTabs);
}

void MainWindow::on_settingsChanged(const std::string &name)
{
	if (name == "CompactLayout")
	{
		doLayout();
	}
}
void MainWindow::doLayout()
{
	if (settingsAccessor->GetBoolByName("CompactLayout", DEFAULT_COMPACTLAYOUT))
	{
		emptyContainers();
		defaultLayout();
	}
	else
	{
		emptyContainers();
		classicLayout();
	}
}

void MainWindow::defaultLayout()
{
	add(hPaned);
	{
		hPaned.pack1(mainVBox);
		{
			mainVBox.pack_start(menubar, Gtk::PACK_SHRINK, 0);
			mainVBox.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
			mainVBox.pack_start(TaskListContainer); // Gtk::AttachOptions(0));

			{
				TaskListContainer.add(taskList);
			}
		}
		hPaned.pack2(secondaryVBox, Gtk::EXPAND | Gtk::SHRINK);
		{
			secondaryVBox.pack_start(calendar, Gtk::PACK_SHRINK, 0);
			secondaryVBox.pack_start(summaryTabs);
			{
				DaySummaryContainer.add(daySummary);
				WeekSummaryContainer.add(weekSummary);
				MonthSummaryContainer.add(monthSummary);
				YearSummaryContainer.add(yearSummary);
				summaryTabs.append_page(DaySummaryContainer, labelDay);
				summaryTabs.append_page(WeekSummaryContainer, labelWeek);
				summaryTabs.append_page(MonthSummaryContainer, labelMonth);
				summaryTabs.append_page(YearSummaryContainer, labelYear);

			}
		}

	}
	//mainVBox.pack_start(statusbar, Gtk::PACK_SHRINK, 0);

}

void MainWindow::classicLayout()
{
	add(mainVBox);
	{
		mainVBox.pack_start(menubar, Gtk::PACK_SHRINK, 0);
		mainVBox.pack_start(toolbar, Gtk::PACK_SHRINK, 0);
		mainVBox.pack_start(hPaned);
		{
			hPaned.pack1(TaskListContainer, Gtk::EXPAND | Gtk::SHRINK); // Gtk::AttachOptions(0));

			{
				TaskListContainer.add(taskList);
			}
			hPaned.pack2(secondaryVBox);
			{
				secondaryVBox.pack_start(calendar, Gtk::PACK_SHRINK, 0);
				secondaryVBox.pack_start(summaryTabs);
				{
					DaySummaryContainer.add(daySummary);
					WeekSummaryContainer.add(weekSummary);
					MonthSummaryContainer.add(monthSummary);
					YearSummaryContainer.add(yearSummary);
					summaryTabs.append_page(DaySummaryContainer, labelDay);
					summaryTabs.append_page(WeekSummaryContainer, labelWeek);
					summaryTabs.append_page(MonthSummaryContainer, labelMonth);
					summaryTabs.append_page(YearSummaryContainer, labelYear);
				}
			}

		}
		mainVBox.pack_start(statusbar, Gtk::PACK_SHRINK, 0);
	}
}

// also displays whether idle
void MainWindow::on_runningTasksChanged()
{
	std::vector<int64_t> taskIDs = timeAccessor->getRunningTasks();
	if (taskIDs.size() > 0)
	{
		set_title("TimeIT ⌚");
	}
	else
	{
		set_title("TimeIT");
	}
	taskList.on_completeUpdate();
}

void MainWindow::on_show()
{
	Gtk::Window::on_show();
	Gtk::Window::deiconify();
	setCalendar();
}

void MainWindow::setCalendar()
{
	struct tm *timeInfo;
	time_t now = libtimeit::now();
	timeInfo = localtime(&now);
	int month = timeInfo->tm_mon;
	int year = timeInfo->tm_year + 1900;
	int day = timeInfo->tm_mday;
	calendar.select_month(month, year);
	calendar.select_day(day);
}

void MainWindow::on_action_task_selection_changed(int selectedTaskID)
{
	toolbar.setTaskIsSelected(selectedTaskID > 0);
	menubar.setTaskIsSelected(selectedTaskID > 0);
}

void MainWindow::on_action_remove_task()
{
	int selectedTaskID = taskList.getSelectedID();
	//Confirm dialog when removing task, headline!
	const char *headline = _("Are you sure you want to delete this task?");
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
		taskAccessor->removeTask(selectedTaskID);
		break;
	case (Gtk::RESPONSE_CANCEL):
		break;
	default:
		cerr << "Unexpected button clicked." << std::endl;
		break;
	}
}

}
