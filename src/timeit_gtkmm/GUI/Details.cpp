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
#include <libtimeit/db/default_values.h>

using namespace Gtk;
using namespace std;
using namespace libtimeit;

namespace GUI
{

Details::Details(
		Database& database,
		Notifier& notifier)
		:
		Event_observer(notifier),
		m_timeAccessor(database),
		m_taskAccessor(database),
		m_settingsAccessor(database)


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

	set_headers_visible(false);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList &menulist = m_Menu_Popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &Details::on_menu_file_popup_edit)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &Details::on_menu_file_popup_merge)));
		Gtk::Menu_Helpers::MenuElem split_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Split"), sigc::mem_fun(*this, &Details::on_menu_file_popup_split));
		m_split_menu_item = split_menu_elem.get_child();
		menulist.push_back(split_menu_elem);
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &Details::on_menu_file_popup_remove)));
	}
	m_Menu_Popup.accelerate(*this);
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Details::on_selection_changed));

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
		optional<Time_entry> optional_time_entry = m_timeAccessor.by_ID(selectedID);
		if (optional_time_entry)
		{
			Time_entry time_entry = optional_time_entry.value();
			int idleGt = m_settingsAccessor.get_int("Gt", DEFAULT_GT);
			int idleGz = m_settingsAccessor.get_int("Gz", DEFAULT_GZ);
			int64_t minutesToLose = difftime(time_entry.stop(), time_entry.start()) / 60;
			std::string minutesString = string_printf("<span color='red'>%d</span>", minutesToLose);
			std::string secondaryText =
				minutesToLose > idleGt || minutesToLose > idleGz || minutesToLose < 0 ?
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
				// coded considering time_entry could be currently running
				if (!time_entry.running())
				{
					m_timeAccessor.remove(selectedID);
				}
				else
				{
					// if running then keep running, starting over with zero length
					m_timeAccessor.update(time_entry.ID(), time_entry.stop(), time_entry.stop());
				}
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
		optional<Time_entry> optional_time_entry_0 = m_timeAccessor.by_ID(selectedIDs[0]);
		optional<Time_entry> optional_time_entry_1 = m_timeAccessor.by_ID(selectedIDs[1]);
		if (optional_time_entry_0 && optional_time_entry_1)
		{
			Time_entry time_entry_0 = optional_time_entry_0.value();
			Time_entry time_entry_1 = optional_time_entry_1.value();
			int idleGt = m_settingsAccessor.get_int("Gt", DEFAULT_GT);
			int idleGz = m_settingsAccessor.get_int("Gz", DEFAULT_GZ);

			int64_t minutesToGain = difftime(time_entry_1.start(), time_entry_0.stop()) / 60;

			std::string minutesString = string_printf(
				minutesToGain >= 0 ? "<span color='green'>%d</span>" : "<span color='red'>%d</span>", minutesToGain);
			std::string secondaryText =
				minutesToGain > idleGt || minutesToGain > idleGz || minutesToGain < 0 ?
				string_printf(
					_("Merging will add %s minutes.\n\nMerging with the next row will be permanent."),
					minutesString.c_str()) :
				_("Merging with the next row will be permanent.");

			Gtk::MessageDialog dialog(_("Do you really want to merge?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondaryText, true);

			int dialog_result = dialog.run();

			//Handle the response:
			switch (dialog_result)
			{
			case (Gtk::RESPONSE_OK):
			{
				// coded considering time_entry_1 could be currently running
				time_t new_start = time_entry_0.start();
				m_timeAccessor.remove(time_entry_0.ID());
				m_timeAccessor.update(time_entry_1.ID(), new_start, time_entry_1.stop());
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

bool offer_to_split(Time_entry &time_entry)
{
	time_t start_time = time_entry.start();
	time_t stop_time = time_entry.stop();
	int64_t seconds_to_split = difftime(stop_time, start_time);
	bool across_days = onDifferentDays(start_time, stop_time);
	// at least use sufficient margins to stay clear of leap seconds, 2 * 3 = 6 is a good minimum
	return across_days || seconds_to_split > 120;
}

void Details::on_menu_file_popup_split()
{
	int64_t selected_id = getSelectedID();
	if (selected_id > 0)
	{
		optional<Time_entry> optional_time_entry = m_timeAccessor.by_ID(selected_id);
		if (optional_time_entry)
		{
			Time_entry time_entry = optional_time_entry.value();
			time_t start_time = time_entry.start();
			time_t stop_time = time_entry.stop();
			bool across_days = onDifferentDays(start_time, stop_time);

			// coded considering time_entry could be currently running

			if (offer_to_split(time_entry))
			{
				int64_t time_id = time_entry.ID();
				int64_t task_id = time_entry.task_ID();
				time_t split_stop_time;
				time_t split_start_time;
				if (across_days)
				{
					// use sufficient margins to stay clear of leap seconds
					struct tm first_day = *localtime(&start_time);
					first_day.tm_hour = 23;
					first_day.tm_min = 59;
					first_day.tm_sec = 57;
					split_stop_time = mktime(&first_day);
					split_start_time = split_stop_time + 6;
				}
				else
				{
					// if on same day then halfway
					split_stop_time = (start_time + (stop_time - start_time) / 2 - 1);
					split_start_time = split_stop_time + 2;
				}
				m_timeAccessor.update(time_id, split_start_time, stop_time);
				m_timeAccessor.create(task_id, start_time, split_stop_time);
				empty();
				populate();
			}
			else
			{
				// don't
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
		int64_t selected_id = getSelectedID();
		if (selected_id > 0)
		{
			optional<Time_entry> optional_time_entry = m_timeAccessor.by_ID(selected_id);
			if (optional_time_entry)
			{
				if (offer_to_split(optional_time_entry.value()))
				{
					m_split_menu_item->set_sensitive(true);
				}
				else
				{
					m_split_menu_item->set_sensitive(false);
				}

				m_Menu_Popup.popup(event->button, event->time);
			}
		}
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
void Details::on_task_updated(int64_t)
{
	populate();
}

void Details::on_task_name_changed(int64_t)
{
	populate();
}

void Details::on_task_time_changed(int64_t)
{
	populate();
}

void Details::on_task_removed(int64_t)
{
	empty();
	populate();
}

void Details::on_complete_update()
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
	std::vector<Time_entry> time_list = m_timeAccessor.time_list(m_taskID, m_startTime, m_stopTime);
	std::vector<Time_entry>::iterator iter = time_list.begin();
	time_t prevStartTime = 0;
	bool first = true;
	int64_t secondsInDay;
	time_t now = time(nullptr);
	bool isToday = !onDifferentDays(m_startTime, now);
	bool isPast = difftime(now, m_stopTime) > 0;
	if (!isToday && !get_column(m_morningColumnN)->get_visible())
	{
		get_column(m_morningColumnN)->set_visible(true);
	}
	for (; iter != time_list.end(); )
	{
		Time_entry te = *iter;
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
		if (++iter != time_list.end()) // here iterate to next
		{
			Time_entry nextTe = *iter;
			time_t nextStartTime = nextTe.start();
			lastOnDay = onDifferentDays(startTime, nextStartTime);
			row[m_columns.m_col_idle] = "\u2003" + createIdlingString(stopTime, nextStartTime);
		}
		else
		{
			// last
			lastOnDay = true;
			row[m_columns.m_col_idle] = string("\u2003") + (te.running() ? "⌚" : (isPast ? "⇣" : ""));
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
