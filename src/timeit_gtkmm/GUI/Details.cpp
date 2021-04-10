/*
 * Details.cpp
 *
 *  Created on: 2008-jul-22
 *      Author: hoglet
 */

#include "Details.h"
#include <vector>
#include <libtimeit/utils.h>
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
		m_settingsAccessor(database)


{
	m_startTime = 0;
	m_stopTime = 0;
	m_taskID = 0;
	m_treeModel = ListStore::create(m_columns);
	set_model(m_treeModel);

	m_morningColumnN = append_column(_(" Day"), m_columns.m_col_morning) - 1;
	get_column(m_morningColumnN)->set_visible(false);
	append_column(_(" Date"), m_columns.m_col_date);
	append_column(_(" Time span"), m_columns.m_col_time);
	append_column(_(" Duration"), m_columns.m_col_time_amount);
	append_column("  ∑  ",m_columns.m_col_day_total);
	append_column(_(" Idle time"), m_columns.m_col_idle);

	set_headers_visible(true);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList &menulist = m_Menu_Popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &Details::on_menu_file_popup_edit)));

		Gtk::Menu_Helpers::MenuElem merge_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &Details::on_menu_file_popup_merge));
		m_merge_menu_item = merge_menu_elem.get_child();
		menulist.push_back(merge_menu_elem);

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

vector<int64_t> Details::getSelectedAndNextID()
{
	vector<int64_t> retVal(2, 0);
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
					m_timeAccessor.update(time_entry);
				}
				auto row_data = create_row_data(m_startTime, m_stopTime);
				populate( row_data );
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

bool offer_to_merge(optional<Time_entry> &optional_time_entry, optional<Time_entry> &optional_next_time_entry)
{
	return optional_time_entry.has_value() && optional_next_time_entry.has_value();
}

void Details::on_menu_file_popup_merge()
{
	vector<int64_t> selectedIDs = getSelectedAndNextID();
	if (selectedIDs[0] > 0 && selectedIDs[1] > 0)
	{
		optional<Time_entry> optional_time_entry_0 = m_timeAccessor.by_ID(selectedIDs[0]);
		optional<Time_entry> optional_time_entry_1 = m_timeAccessor.by_ID(selectedIDs[1]);
		if (offer_to_merge(optional_time_entry_0, optional_time_entry_1))
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
				m_timeAccessor.update( time_entry_1.with_start(new_start) );
				auto row_data = create_row_data(m_startTime, m_stopTime);
				populate( row_data );
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
				m_timeAccessor.update( time_entry.with_start(split_start_time).with_stop(stop_time) );
				m_timeAccessor.create( Time_entry(task_id, start_time, split_stop_time) );
				auto row_data = create_row_data(m_startTime, m_stopTime);
				populate( row_data );
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
		vector<int64_t> selected_ids = getSelectedAndNextID();
		int64_t selected_id = selected_ids[0];
		int64_t next_id = selected_ids[1];
		if (selected_id > 0)
		{
			optional<Time_entry> optional_time_entry = m_timeAccessor.by_ID(selected_id);
			optional<Time_entry> optional_next_time_entry = m_timeAccessor.by_ID(next_id);
			if (optional_time_entry)
			{
				if (offer_to_merge(optional_time_entry, optional_next_time_entry))
				{
					m_merge_menu_item->set_sensitive(true);
				}
				else
				{
					m_merge_menu_item->set_sensitive(false);
				}

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

optional<Gtk::TreeModel::Row> Details::findRow(int id)
{
	TreeModel::Children children = m_treeModel->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[m_columns.m_col_id] == id))
		{
			return *iter;
		}
	}
	return {};
}
void Details::on_task_updated(int64_t)
{
	auto row_data = create_row_data(m_startTime, m_stopTime);
	populate( row_data );
}

void Details::on_task_name_changed(int64_t)
{
	auto row_data = create_row_data(m_startTime, m_stopTime);
	populate( row_data );
}

void Details::on_time_entry_changed(int64_t ID)
{
	auto row = findRow(ID);
	if ( row.has_value() )
	{
		auto time_entry = m_timeAccessor.by_ID(ID);
		if(time_entry.has_value())
		{
			auto day = time_entry->start();
			auto start_time = getBeginingOfDay(day);
			auto stop_time = getEndOfDay(day);
			auto row_data = create_row_data( start_time, stop_time);
			update(row_data);
		}
	}
	else
	{
		auto row_data = create_row_data(m_startTime, m_stopTime);
		populate( row_data );
	}
}

void Details::on_task_removed(int64_t)
{
	auto row_data = create_row_data(m_startTime, m_stopTime);
	populate( row_data );
}

void Details::on_complete_update()
{
	auto row_data = create_row_data(m_startTime, m_stopTime);
	populate( row_data );
}

void Details::set(int64_t ID, time_t startTime, time_t stopTime)
{
	bool acrossDays = onDifferentDays(startTime, stopTime);
	if (ID > 0)
	{
		m_taskID = ID;
		m_startTime = startTime;
		m_stopTime = stopTime;
		get_column(m_morningColumnN)->set_visible(acrossDays);

		auto row_data = create_row_data(m_startTime, m_stopTime);
		populate( row_data );
	}
	else
	{
		m_taskID = 0;
		m_startTime = 0;
		m_stopTime = 0;
		m_treeModel->clear();
	}
}





void Details::update_row(TreeModel::Row& row, Row_data row_data )
{
	row[m_columns.m_col_id]          = row_data.time_ID;
	row[m_columns.m_col_morning]     = row_data.first_in_day ? dayOfWeekAbbreviation(row_data.start) : "";
	row[m_columns.m_col_date]        = row_data.first_in_day ? createDateString(row_data.start) : "";
	row[m_columns.m_col_time]        = create_time_span_string(row_data.start, row_data.stop);
	row[m_columns.m_col_time_amount] = create_duration_string(row_data.start, row_data.stop);
	if(row_data.running)
	{
		row[m_columns.m_col_idle] = string("\u2003⌚");
	}
	else if( row_data.next_start >= row_data.stop)
	{
		row[m_columns.m_col_idle] = string("\u2003") + createIdlingString(row_data.stop, row_data.next_start);
	}
	else
	{
		row[m_columns.m_col_idle] = string("\u2003") + "⇣";
	}
	if(row_data.last_in_day)
	{
		auto seconds_today = row_data.cumulative_time;
		row[m_columns.m_col_day_total] = "\u2003" + string(onDifferentDays(row_data.start, row_data.stop) ? "≠" : "≈")
										 + seconds2hhmm(seconds_today) + " ";
	}
}


list<Row_data> Details::create_row_data(time_t start, time_t stop)
{
	Time_list time_list = m_timeAccessor.time_list(m_taskID, start, stop);
	time_t prev_start   = 0;
	auto iter = time_list.begin();
	list<Row_data> data_rows = {};
	int cumulative_time_for_day = 0;
	for (; iter != time_list.end(); )
	{
		auto time_entry = *iter;
		Row_data row_data;

		row_data.time_ID      = time_entry.ID();
		row_data.prev_start   = prev_start;
		row_data.start        = time_entry.start();
		prev_start            = row_data.start;
		row_data.stop         = time_entry.stop();
		row_data.running      = time_entry.running();

		cumulative_time_for_day += row_data.stop - row_data.start;
		row_data.cumulative_time =  cumulative_time_for_day;

		++iter;
		if( iter != time_list.end() )
		{
			auto next_time_entry = *iter;
			row_data.next_start = next_time_entry.start();
			row_data.first_in_day = onDifferentDays(row_data.prev_start, row_data.start);
			row_data.last_in_day = onDifferentDays(row_data.start, row_data.next_start);
			if(row_data.last_in_day)
			{
				cumulative_time_for_day = 0;
			}
		}
		else
		{
			row_data.first_in_day = onDifferentDays( row_data.prev_start, row_data.start);
			row_data.last_in_day = true;
			row_data.next_start = 0;
		}
		data_rows.push_back(row_data);
	}
	return data_rows;
}

void Details::populate(list<Row_data> data_rows)
{
	m_treeModel->clear();

	for (auto row_data: data_rows )
	{
		auto treeIter = m_treeModel->append();
		TreeModel::Row row = *treeIter;

		update_row(row, row_data );

	}
}

void Details::update(list<Row_data> data_rows)
{

	for (auto row_data: data_rows )
	{
		//auto treeIter = m_treeModel->append();
		auto row = findRow(row_data.time_ID);
		if ( row.has_value() )
		{
			update_row(*row, row_data);
		}
		else
		{
			on_complete_update();
		}

	}
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
