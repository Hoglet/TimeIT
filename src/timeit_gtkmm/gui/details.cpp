#include "details.h"
#include <fmt/core.h>
#include <vector>
#include <libtimeit/utils.h>
#include <glibmm/i18n.h>
#include <optional>
#include <libtimeit/db/default_values.h>
#include <edit_time.h>

using namespace Gtk;
using namespace std;
using namespace libtimeit;

namespace gui
{

static const int SECONDS_PER_MINUTE = 60;

details::details(
		database& database_,
		notification_manager& notifier,
		window_manager&  gui_factory_)
		:
		event_observer(notifier),
		times(database_),
		settings(database_),
		windows(gui_factory_),
		db(database_)
{
	tree_model = ListStore::create(columns);
	set_model(tree_model);

	append_column(_(" Day"), columns.col_morning);
	append_column(_(" Date"), columns.col_date);
	append_column(_(" Time span"), columns.col_time);
	append_column(_(" Duration"), columns.col_time_amount);
	append_column("  ∑  ", columns.col_day_total);
	append_column(_(" Idle time"), columns.col_idle);
	append_column(_(" Comment"), columns.col_comment);

	set_headers_visible(false);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList &menulist = menu_popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &details::on_menu_file_popup_edit)));

		Gtk::Menu_Helpers::MenuElem merge_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &details::on_menu_file_popup_merge));
		merge_menu_item = merge_menu_elem.get_child();
		menulist.push_back(merge_menu_elem);

		Gtk::Menu_Helpers::MenuElem split_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Split"), sigc::mem_fun(*this, &details::on_menu_file_popup_split));
		split_menu_item = split_menu_elem.get_child();
		menulist.push_back(split_menu_elem);

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &details::on_menu_file_popup_remove)));
	}
	menu_popup.accelerate(*this);
}


optional<time_id> details::get_selected()
{
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return row[columns.col_id];

	}
	return {};
}

list<time_id> details::get_selected_and_next()
{
	list<time_id> return_value;
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return_value.push_back( row[columns.col_id]);
		//std::cout << "one row" << std::endl;
		++iter;
		if (iter) // if there is a next row
		{
			TreeModel::Row next_row = *iter;
			return_value.push_back(next_row[columns.col_id]);
			//std::cout << "two rows" << std::endl;
		}
	}
	return return_value;
}


void details::on_menu_file_popup_remove()
{
	auto selected = get_selected();
	if (selected.has_value() )
	{
		optional<Time_entry> optional_time_entry = times.by_id(selected.value());
		if (optional_time_entry)
		{
			Time_entry time_entry = optional_time_entry.value();
			auto idle_gt = settings.get_int("Gt", DEFAULT_GT);
			auto idle_gz = settings.get_int("Gz", DEFAULT_GZ);
			int64_t minutes_to_lose = (int64_t)difftime(time_entry.stop, time_entry.start) / SECONDS_PER_MINUTE;
			std::string minutes_string = fmt::format("<span color='red'>{}</span>", minutes_to_lose);
			std::string secondary_text;
			if (minutes_to_lose > idle_gt || minutes_to_lose > idle_gz || minutes_to_lose < 0)
			{
				secondary_text = fmt::format(
						_("Removing will lose {} minutes.\n\nRemoving will be permanent."),
						minutes_string.c_str());
			}
			else
			{
				secondary_text = _("Gone, gone will not come again...");
			}

			Gtk::MessageDialog dialog(_("Do you really want to remove this?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondary_text, true);

			int result = dialog.run();

			//Handle the response:
			switch (result)
			{
			case (Gtk::RESPONSE_OK):
			{
				// coded considering time_entry could be currently running
				if ( time_entry.state == RUNNING )
				{
					// if running then keep running, starting over with zero length
					times.update(time_entry.with_start(time_entry.stop));
				}
				else
				{
					times.remove(time_entry);
				}
				populate( );
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				[[fallthrough]];
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

void details::on_menu_file_popup_merge()
{
	auto selected_list = get_selected_and_next();
	if (selected_list.size()>1)
	{
		optional<Time_entry> optional_time_entry_0 = times.by_id(selected_list.front());
		optional<Time_entry> optional_time_entry_1 = times.by_id(selected_list.back());
		if (offer_to_merge(optional_time_entry_0, optional_time_entry_1))
		{
			Time_entry time_entry_0 = optional_time_entry_0.value();
			Time_entry time_entry_1 = optional_time_entry_1.value();
			auto idle_gt = settings.get_int("Gt", DEFAULT_GT);
			auto idle_gz = settings.get_int("Gz", DEFAULT_GZ);

			auto minutes_to_gain = (int)difftime(time_entry_1.start, time_entry_0.stop) / SECONDS_PER_MINUTE;

			std::string minutes_string;
			if (minutes_to_gain >= 0)
			{
				minutes_string = fmt::format("<span color='green'>{}</span>", minutes_to_gain);
			}
			else
			{
				minutes_string = fmt::format("<span color='red'>{}</span>", minutes_to_gain);
			}
			std::string secondary_text;
			if (
					minutes_to_gain > idle_gt ||
					minutes_to_gain > idle_gz ||
					minutes_to_gain < 0)
			{
				secondary_text = fmt::format(
						_("Merging will add {} minutes.\n\nMerging with the next row will be permanent."),
						minutes_string.c_str());
			}
			else
			{
				secondary_text = _("Merging with the next row will be permanent.");
			}

			Gtk::MessageDialog dialog(_("Do you really want to merge?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondary_text, true);

			int dialog_result = dialog.run();

			//Handle the response:
			switch (dialog_result)
			{
			case (Gtk::RESPONSE_OK):
			{
				// coded considering time_entry_1 could be currently running
				time_t new_start = time_entry_0.start;
				times.remove(time_entry_0);
				times.update(time_entry_1.with_start(new_start) );
				populate( );
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				[[fallthrough]];
			default:
				break;
			}
		}
	}
}

bool offer_to_split(Time_entry &time_entry)
{
	time_t start_time = time_entry.start;
	time_t stop_time = time_entry.stop;
	auto seconds_to_split = (long)difftime(stop_time, start_time);
	bool across_days = is_on_different_days(start_time, stop_time);
	// at least use sufficient margins to stay clear of leap seconds, 2 * 3 = 6 is a good minimum
	return across_days || seconds_to_split > 120; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void details::on_menu_file_popup_split()
{
	auto selected = get_selected();
	if (selected.has_value())
	{
		optional<Time_entry> optional_time_entry = times.by_id(selected.value());
		if (optional_time_entry)
		{
			Time_entry time_entry = optional_time_entry.value();
			time_t start = time_entry.start;
			time_t stop = time_entry.stop;
			bool across_days = is_on_different_days(start, stop);

			// coded considering time_entry could be currently running

			if (offer_to_split(time_entry))
			{
				time_t split_stop_time  = (start + (stop - start) / 2 - 1);
				time_t split_start_time = split_stop_time + 2;
				if (across_days)
				{
					// use sufficient margins to stay clear of leap seconds
					struct tm first_day = *localtime(&start);
					first_day.tm_hour = 23;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					first_day.tm_min = 59;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					first_day.tm_sec = 57;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					split_stop_time = mktime(&first_day);
					split_start_time = split_stop_time + 6; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
				}

				times.update(time_entry.with_start(split_start_time).with_stop(stop) );
				times.create(Time_entry(time_entry.owner, start, split_stop_time) );
				populate( create_row_data(start, stop) );
			}
			else
			{
				// don't
			}
		}
	}
}

bool details::on_button_press_event(GdkEventButton *event)
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = TreeView::on_button_press_event(event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		auto selected_ids = get_selected_and_next();
		if(selected_ids.size()==2)
		{
			auto selected_id = selected_ids.front();
			auto next_id = selected_ids.back();
			optional<Time_entry> optional_time_entry = times.by_id(selected_id);
			optional<Time_entry> optional_next_time_entry = times.by_id(next_id);
			if (optional_time_entry)
			{
				if (offer_to_merge(optional_time_entry, optional_next_time_entry))
				{
					merge_menu_item->set_sensitive(true);
				}
				else
				{
					merge_menu_item->set_sensitive(false);
				}

				if (offer_to_split(optional_time_entry.value()))
				{
					split_menu_item->set_sensitive(true);
				}
				else
				{
					split_menu_item->set_sensitive(false);
				}

				menu_popup.popup(event->button, event->time);
			}
		}
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		auto selected = get_selected();
		if(selected.has_value())
		{
			edit_time_entry(selected.value());
			return_value = true;
		}
	}

	return return_value;
}

optional<Gtk::TreeModel::Row> details::find_row(time_id id)
{
	TreeModel::Children children = tree_model->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[columns.col_id] == id))
		{
			return *iter;
		}
	}
	return {};
}
void details::on_task_updated(Task_id /*id*/)
{
	populate( );
}

void details::on_task_name_changed(Task_id /*id*/)
{
	populate( );
}

void details::on_time_entry_changed(const Time_entry& te)
{
	auto row = find_row(te.uuid);
	if ( row.has_value() )
	{
		auto time_entry = times.by_id(te.uuid);
		if(time_entry.has_value())
		{
			auto day = time_entry->start;
			auto start = beginning_of_day(day);
			auto stop = end_of_day(day);
			update( create_row_data( start, stop) );
		}
	}
	else
	{
		populate( );
	}
}

void details::on_task_removed(Task_id /*id*/)
{
	populate();
}

void details::on_complete_update()
{
	populate( );
}

void details::set(int64_t ID, time_t start, time_t stop)
{
	if (ID > 0)
	{
		set_headers_visible(true);
		presented_task = ID;
		start_time = start;
		stop_time = stop;
		populate( );
	}
	else
	{
		set_headers_visible(false);
		presented_task = 0;
		start_time = 0;
		stop_time = 0;
		tree_model->clear();
	}
}

void details::on_selection_changed(int64_t id, time_t start, time_t stop)
{
	set(id, start, stop);
}



void details::update_row(TreeModel::Row& row, row_data data ) const
{
	row[columns.col_id]          = data.id;
	row[columns.col_morning]     = data.first_in_day ? day_of_week_abbreviation(data.start) : "";
	row[columns.col_date]        = data.first_in_day ? date_string(data.start) : "";
	row[columns.col_time]        = time_span_string(data.start, data.stop);
	row[columns.col_time_amount] = duration_string(data.start, data.stop);
	row[columns.col_comment]     = libtimeit::abbreviate_string(data.comment, 50);
	if(data.running)
	{
		row[columns.col_idle] = string("\u2003⌚");
	}
	else if( data.next_start >= data.stop)
	{
		row[columns.col_idle] = string("\u2003") + idling_string(data.stop, data.next_start);
	}
	else
	{
		row[columns.col_idle] = string("\u2003") + "⇣";
	}
	if(data.last_in_day)
	{
		auto seconds_today = data.cumulative_time;
		if (is_on_different_days(data.start, data.stop))
		{
			row[columns.col_day_total] = fmt::format("\u2003≠{} ", seconds_2_hhmm(seconds_today).c_str() );
		}
		else
		{
			row[columns.col_day_total] = fmt::format("\u2003≈{} ", seconds_2_hhmm(seconds_today).c_str());
		}
	}
}


list<row_data> details::create_row_data(time_t start, time_t stop)
{
	Time_list time_list = times.time_list(presented_task, start, stop);
	time_t prev_start   = 0;
	auto iter = time_list.begin();
	list<row_data> data_rows = {};
	time_t cumulative_time_for_day = 0;
	for (; iter != time_list.end(); )
	{
		auto time_entry = *iter;
		row_data data{};

		data.id           = time_entry.uuid;
		data.prev_start   = prev_start;
		data.start        = time_entry.start;
		prev_start            = data.start;
		data.stop         = time_entry.stop;
		data.running      = time_entry.state == RUNNING;
		data.comment      = time_entry.comment;

		cumulative_time_for_day += data.stop - data.start;
		data.cumulative_time =  cumulative_time_for_day;

		++iter;
		if( iter != time_list.end() )
		{
			auto next_time_entry = *iter;
			data.next_start = next_time_entry.start;
			data.first_in_day = is_on_different_days(data.prev_start, data.start);
			data.last_in_day = is_on_different_days(data.start, data.next_start);
			if(data.last_in_day)
			{
				cumulative_time_for_day = 0;
			}
		}
		else
		{
			data.first_in_day = is_on_different_days(data.prev_start, data.start);
			data.last_in_day = true;
			data.next_start = 0;
		}
		data_rows.push_back(data);
	}
	return data_rows;
}

void details::populate()
{
	populate( create_row_data(start_time, stop_time) );
}
void details::populate(list<row_data> data_rows)
{
	tree_model->clear();

	for (auto data: data_rows )
	{
		auto tree_iter = tree_model->append();
		TreeModel::Row row = *tree_iter;

		update_row(row, data );

	}
}

void details::update(list<row_data> data_rows)
{

	for (auto data: data_rows )
	{
		//auto treeIter = m_treeModel->append();
		auto row = find_row(data.id);
		if ( row.has_value() )
		{
			update_row(*row, data);
		}
		else
		{
			on_complete_update();
		}

	}
}



void details::on_menu_file_popup_edit()
{
	auto selected = get_selected();
	if (selected.has_value())
	{
		edit_time_entry(selected.value());
	}
}



void details::edit_time_entry(time_id id)
{
	auto time_entry = times.by_id(id);
	if(time_entry.has_value())
	{
		auto dialog = make_shared<gui::edit_time_dialog>(*time_entry, db);
		windows.manage_lifespan(dialog);
		dialog->show();
	}
}

}
