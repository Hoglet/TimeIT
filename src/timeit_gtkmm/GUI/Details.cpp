/*
 * Details.cpp
 *
 *  Created on: 2008-jul-22
 *      Author: hoglet
 */

#include "Details.h"
#include <vector>
#include <libtimeit/Utils.h>
#include <glibmm/i18n.h>
#include <optional>
#include <vector>
#include <libtimeit/db/DefaultValues.h>

using namespace Gtk;
using namespace std;
using namespace libtimeit;

namespace GUI
{

Details::Details(IDatabase &database):
		m_timeAccessor(database.getTimeAccessor()),
		m_taskAccessor(database.getTaskAccessor()),
		m_settingsAccessor(database.getSettingsAccessor())

{
	m_calendar = nullptr;
	m_startTime = 0;
	m_stopTime = 0;
	m_taskID = 0;
	m_treeModel = ListStore::create(m_columns);
	set_model(m_treeModel);
	m_morningColumnN = append_column("Morning", m_columns.m_col_morning) - 1;
	get_column(m_morningColumnN)->set_visible(false);
	append_column("Time", m_columns.m_col_time);
	append_column("Evening", m_columns.m_col_evening);
	append_column("Idle", m_columns.m_col_idle);
	m_taskAccessor->attach(this);
	set_headers_visible(false);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList &menulist = m_Menu_Popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &Details::on_menu_file_popup_edit)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &Details::on_menu_file_popup_merge)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &Details::on_menu_file_popup_remove)));
	}
	m_Menu_Popup.accelerate(*this);
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Details::on_selection_changed));

}

Details::~Details()
{
	m_taskAccessor->detach(this);
}

int64_t Details::getSelectedID()
{
	int retVal = 0;
	Glib::RefPtr<TreeSelection> refTreeSelection = get_selection();
	TreeModel::iterator iter;
	iter = refTreeSelection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		retVal = row[m_columns.m_col_id];
	}
	return retVal;
}

std::vector<int64_t> Details::getSelectedAndNextID()
{
	std::vector<int64_t> retVal(2, 0);
	Glib::RefPtr<TreeSelection> refTreeSelection = get_selection();
	TreeModel::iterator iter;
	iter = refTreeSelection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		retVal[0] = row[m_columns.m_col_id];
		//std::cout << "one row" << std::endl;
		++iter;
		if (iter) // if there is a next row
		{
			TreeModel::Row nextRow = *iter;
			retVal[1] = nextRow[m_columns.m_col_id];
			//std::cout << "two rows" << std::endl;
		}
	}
	return retVal;
}

void Details::on_menu_file_popup_remove()
{
	int64_t selectedID = getSelectedID();
	if (selectedID > 0)
	{
		optional<TimeEntry> ote = m_timeAccessor->getByID(selectedID);
		if (ote)
		{
			TimeEntry te = ote.value();
			int idleGt = m_settingsAccessor->GetIntByName("Gt", DEFAULT_GT);
			int idleGz = m_settingsAccessor->GetIntByName("Gz", DEFAULT_GZ);
			int64_t minutesToLose = difftime(te.stop(), te.start()) / 60;
			std::string minutesString = string_printf("<span color='red'>%d</span>", minutesToLose);
			std::string secondaryText =
				minutesToLose > idleGt || minutesToLose > idleGz ?
				string_printf(
					_("Removing will lose %s minutes.\n\nRemoving will be permanent."),
					minutesString.c_str()) :
				_("Gone, gone will not come again...");

			Gtk::MessageDialog dialog(_("Do you really want to remove this?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondaryText, true);

			int result = dialog.run();

			//Handle the response:
			switch (result)
			{
			case (Gtk::RESPONSE_OK):
			{
				m_timeAccessor->remove(selectedID);
				empty();
				populate();
				//DetailsDialog::instance().show();
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				//std::cout << "Cancel clicked." << std::endl;
				break;
			default:
				//std::cout << "Unexpected button clicked." << std::endl;
				break;
			}
		}
	}
}

void Details::on_menu_file_popup_merge()
{
	std::vector<int64_t> selectedIDs = getSelectedAndNextID();
	if (selectedIDs[0] > 0 && selectedIDs[1] > 0)
	{
		optional<TimeEntry> ote0 = m_timeAccessor->getByID(selectedIDs[0]);
		optional<TimeEntry> ote1 = m_timeAccessor->getByID(selectedIDs[1]);
		if (ote0 && ote1)
		{
			TimeEntry te0 = ote0.value();
			TimeEntry te1 = ote1.value();
			int idleGt = m_settingsAccessor->GetIntByName("Gt", DEFAULT_GT);
			int idleGz = m_settingsAccessor->GetIntByName("Gz", DEFAULT_GZ);
			int64_t minutesToGain = difftime(te1.start(), te0.stop()) / 60;
			std::string minutesString = string_printf("<span color='green'>%d</span>", minutesToGain);
			std::string secondaryText =
				minutesToGain > idleGt || minutesToGain > idleGz ?
				string_printf(
					_("Merging will add %s minutes.\n\nMerging with the next row will be permanent."),
					minutesString.c_str()) :
				_("Merging with the next row will be permanent.");

			Gtk::MessageDialog dialog(_("Do you really want to merge?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondaryText, true);

			int result = dialog.run();

			//Handle the response:
			switch (result)
			{
			case (Gtk::RESPONSE_OK):
			{
				time_t newStart = te0.start();
				m_timeAccessor->remove(te0.ID());
				m_timeAccessor->updateTime(te1.ID(), newStart, te1.stop());
				empty();
				populate();
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				break;
			default:
				break;
			}
		}
	}
}

bool Details::on_button_press_event(GdkEventButton *event)
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = TreeView::on_button_press_event(event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		m_Menu_Popup.popup(event->button, event->time);
	}

	return return_value;
}

Gtk::TreeModel::iterator Details::findRow(int id)
{
	TreeModel::Children children = m_treeModel->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[m_columns.m_col_id] == id))
		{
			break;
		}
	}
	return iter;
}
void Details::on_taskUpdated(int64_t)
{
	populate();
}

void Details::on_taskNameChanged(int64_t)
{
	populate();
}

void Details::on_taskTimeChanged(int64_t)
{
	populate();
}

void Details::on_taskRemoved(int64_t)
{
	empty();
	populate();
}

void Details::on_completeUpdate()
{
	empty();
	populate();
}

void Details::set(int64_t ID, time_t startTime, time_t stopTime)
{
	bool acrossDays = onDifferentDays(startTime, stopTime);
	if (ID > 0)
	{
		m_taskID = ID;
		m_startTime = startTime;
		m_stopTime = stopTime;
		empty();
		get_column(m_morningColumnN)->set_visible(acrossDays);
		populate();
	}
	else
	{
		m_taskID = 0;
		m_startTime = 0;
		m_stopTime = 0;
		empty();
	}
}
void Details::empty()
{
	m_treeModel->clear();
}

void Details::populate()
{
	std::vector<TimeEntry> timeList = m_timeAccessor->getDetailTimeList(m_taskID, m_startTime, m_stopTime);
	std::vector<TimeEntry>::iterator iter = timeList.begin();
	time_t prevStartTime = 0;
	bool first = true;
	int64_t secondsInDay;
	time_t now = time(nullptr);
	bool isToday = !onDifferentDays(m_startTime, now);
	if (!isToday && !get_column(m_morningColumnN)->get_visible())
	{
		get_column(m_morningColumnN)->set_visible(true);
	}
	for (; iter != timeList.end(); )
	{
		TimeEntry te = *iter;
		TreeModel::Row row;
		TreeModel::iterator TMIter;
		Gtk::TreeIter treeIter = findRow(te.ID());
		time_t startTime = te.start();
		time_t stopTime = te.stop();
		bool firstOnDay = onDifferentDays(prevStartTime, startTime);
		bool lastOnDay;
		if (treeIter == m_treeModel->children().end())
		{
			treeIter = m_treeModel->append();
		}
		row = *treeIter;
		row[m_columns.m_col_id] = te.ID();
		row[m_columns.m_col_morning] = firstOnDay ? dayOfWeekAbbreviation(startTime) : "";
		row[m_columns.m_col_time] = createDurationString(startTime, stopTime);
		if (++iter != timeList.end()) // here iterate to next
		{
			TimeEntry nextTe = *iter;
			time_t nextStartTime = nextTe.start();
			lastOnDay = onDifferentDays(startTime, nextStartTime);
			row[m_columns.m_col_idle] = "\u2003" + createIdlingString(stopTime, nextStartTime);
		}
		else
		{
			// last
			lastOnDay = true;
			row[m_columns.m_col_idle] = "\u2003⋯";
		}
		secondsInDay = (firstOnDay ? 0 : secondsInDay) +
			// within limits only instead of simply difftime(stopTime, startTime)
			difftime(
				difftime(m_stopTime, stopTime) > 0 ? stopTime : m_stopTime,
				difftime(startTime, m_startTime) > 0 ? startTime : m_startTime);
		if (lastOnDay) {
			row[m_columns.m_col_evening] =
				(first && onDifferentDays(startTime, m_startTime) ? "≠" : "≈") + seconds2hhmm(secondsInDay);
		}
		else
		{
			row[m_columns.m_col_evening] = "";
		}
		prevStartTime = startTime;
		first = false;
	}
	columns_autosize();
}

void Details::on_menu_file_popup_edit()
{
	int64_t selectedID = getSelectedID();
	if (selectedID > 0)
	{
		std::list<DetailsObserver*>::iterator iter;
		for (iter = observers.begin(); iter != observers.end(); ++iter)
		{
			DetailsObserver *observer = *iter;
			observer->on_edit_details(selectedID);
			//m_detailsDialog.setTimeEntryID(selectedID);
		}
	}
}

void Details::on_selection_changed()
{
	std::list<DetailsObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		DetailsObserver *observer = *iter;
		observer->on_selected_changed();
	}
}

void Details::attach(DetailsObserver *observer)
{
	observers.push_back(observer);
}
void Details::detach(DetailsObserver *observer)
{
	observers.remove(observer);
}

}
