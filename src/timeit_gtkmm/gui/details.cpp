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

Details::Details(
		Database& database_,
		Notifier& notifier,
		Window_manager&  gui_factory_)
		:
		Event_observer(notifier),
		time_accessor(database_),
		settings_accessor(database_),
		window_manager(gui_factory_),
		database(database_)
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

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &Details::on_menu_file_popup_edit)));

		Gtk::Menu_Helpers::MenuElem merge_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &Details::on_menu_file_popup_merge));
		merge_menu_item = merge_menu_elem.get_child();
		menulist.push_back(merge_menu_elem);

		Gtk::Menu_Helpers::MenuElem split_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Split"), sigc::mem_fun(*this, &Details::on_menu_file_popup_split));
		split_menu_item = split_menu_elem.get_child();
		menulist.push_back(split_menu_elem);

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &Details::on_menu_file_popup_remove)));
	}
	menu_popup.accelerate(*this);
}


Time_id Details::get_selected_id()
{
	Time_id return_value = 0;
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return_value = row[columns.col_id];
	}
	return return_value;
}

vector<Time_id> Details::get_selected_and_next_id()
{
	vector<Time_id> return_value(2, 0);
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return_value[0] = row[columns.col_id];
		//std::cout << "one row" << std::endl;
		++iter;
		if (iter) // if there is a next row
		{
			TreeModel::Row next_row = *iter;
			return_value[1] = next_row[columns.col_id];
			//std::cout << "two rows" << std::endl;
		}
	}
	return return_value;
}


void Details::on_menu_file_popup_remove()
{
	Time_id selected_id = get_selected_id();
	if (selected_id > 0)
	{
		optional<Time_entry> optional_time_entry = time_accessor.by_id(selected_id);
		if (optional_time_entry)
		{
			Time_entry time_entry = optional_time_entry.value();
			auto idle_gt = settings_accessor.get_int("Gt", DEFAULT_GT);
			auto idle_gz = settings_accessor.get_int("Gz", DEFAULT_GZ);
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
					time_accessor.update(time_entry.with_start(time_entry.stop));
				}
				else
				{
					time_accessor.remove(selected_id);
				}
				auto row_data = create_row_data(start_time, stop_time);
				populate( row_data );
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

void Details::on_menu_file_popup_merge()
{
	vector<Time_id> selected_id_list = get_selected_and_next_id();
	if (selected_id_list[0] > 0 && selected_id_list[1] > 0)
	{
		optional<Time_entry> optional_time_entry_0 = time_accessor.by_id(selected_id_list[0]);
		optional<Time_entry> optional_time_entry_1 = time_accessor.by_id(selected_id_list[1]);
		if (offer_to_merge(optional_time_entry_0, optional_time_entry_1))
		{
			Time_entry time_entry_0 = optional_time_entry_0.value();
			Time_entry time_entry_1 = optional_time_entry_1.value();
			auto idle_gt = settings_accessor.get_int("Gt", DEFAULT_GT);
			auto idle_gz = settings_accessor.get_int("Gz", DEFAULT_GZ);

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
				time_accessor.remove(time_entry_0.id);
				time_accessor.update(time_entry_1.with_start(new_start) );
				auto row_data = create_row_data(start_time, stop_time);
				populate( row_data );
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

void Details::on_menu_file_popup_split()
{
	int64_t selected_id = get_selected_id();
	if (selected_id > 0)
	{
		optional<Time_entry> optional_time_entry = time_accessor.by_id(selected_id);
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

				time_accessor.update(time_entry.with_start(split_start_time).with_stop(stop) );
				time_accessor.create(Time_entry(time_entry.task_id, start, split_stop_time) );
				auto row_data = create_row_data(start, stop);
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
		vector<int64_t> selected_ids = get_selected_and_next_id();
		int64_t selected_id = selected_ids[0];
		int64_t next_id = selected_ids[1];
		if (selected_id > 0)
		{
			optional<Time_entry> optional_time_entry = time_accessor.by_id(selected_id);
			optional<Time_entry> optional_next_time_entry = time_accessor.by_id(next_id);
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
		edit_time_entry(get_selected_id());
		return_value = true;
	}

	return return_value;
}

optional<Gtk::TreeModel::Row> Details::find_row(Time_id id)
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
void Details::on_task_updated(Task_id /*id*/)
{
	auto row_data = create_row_data(start_time, stop_time);
	populate( row_data );
}

void Details::on_task_name_changed(Task_id /*id*/)
{
	auto row_data = create_row_data(start_time, stop_time);
	populate( row_data );
}

void Details::on_time_entry_changed(Time_id id)
{
	auto row = find_row(id);
	if ( row.has_value() )
	{
		auto time_entry = time_accessor.by_id(id);
		if(time_entry.has_value())
		{
			auto day = time_entry->start;
			auto start = beginning_of_day(day);
			auto stop = end_of_day(day);
			auto row_data = create_row_data( start, stop);
			update(row_data);
		}
	}
	else
	{
		auto row_data = create_row_data(start_time, stop_time);
		populate( row_data );
	}
}

void Details::on_task_removed(Task_id /*id*/)
{
	auto row_data = create_row_data(start_time, stop_time);
	populate( row_data );
}

void Details::on_complete_update()
{
	auto row_data = create_row_data(start_time, stop_time);
	populate( row_data );
}

void Details::set(int64_t ID, time_t start, time_t stop)
{
	if (ID > 0)
	{
		set_headers_visible(true);
		task_id = ID;
		start_time = start;
		stop_time = stop;
		auto row_data = create_row_data(start_time, stop_time);
		populate( row_data );
	}
	else
	{
		set_headers_visible(false);
		task_id = 0;
		start_time = 0;
		stop_time = 0;
		tree_model->clear();
	}
}

void Details::on_selection_changed(int64_t id, time_t start, time_t stop)
{
	set(id, start, stop);
}



void Details::update_row(TreeModel::Row& row, Row_data row_data ) const
{
	row[columns.col_id]          = row_data.time_id;
	row[columns.col_morning]     = row_data.first_in_day ? day_of_week_abbreviation(row_data.start) : "";
	row[columns.col_date]        = row_data.first_in_day ? date_string(row_data.start) : "";
	row[columns.col_time]        = time_span_string(row_data.start, row_data.stop);
	row[columns.col_time_amount] = duration_string(row_data.start, row_data.stop);
	row[columns.col_comment]     = libtimeit::abbreviate_string(row_data.comment, 50);
	if(row_data.running)
	{
		row[columns.col_idle] = string("\u2003⌚");
	}
	else if( row_data.next_start >= row_data.stop)
	{
		row[columns.col_idle] = string("\u2003") + idling_string(row_data.stop, row_data.next_start);
	}
	else
	{
		row[columns.col_idle] = string("\u2003") + "⇣";
	}
	if(row_data.last_in_day)
	{
		auto seconds_today = row_data.cumulative_time;
		if (is_on_different_days(row_data.start, row_data.stop))
		{
			row[columns.col_day_total] = fmt::format("\u2003≠{} ", seconds_2_hhmm(seconds_today).c_str() );
		}
		else
		{
			row[columns.col_day_total] = fmt::format("\u2003≈{} ", seconds_2_hhmm(seconds_today).c_str());
		}
	}
}


list<Row_data> Details::create_row_data(time_t start, time_t stop)
{
	Time_list time_list = time_accessor.time_list(task_id, start, stop);
	time_t prev_start   = 0;
	auto iter = time_list.begin();
	list<Row_data> data_rows = {};
	time_t cumulative_time_for_day = 0;
	for (; iter != time_list.end(); )
	{
		auto time_entry = *iter;
		Row_data row_data{};

		row_data.time_id      = time_entry.id;
		row_data.prev_start   = prev_start;
		row_data.start        = time_entry.start;
		prev_start            = row_data.start;
		row_data.stop         = time_entry.stop;
		row_data.running      = time_entry.state == RUNNING;
		row_data.comment      = time_entry.comment;

		cumulative_time_for_day += row_data.stop - row_data.start;
		row_data.cumulative_time =  cumulative_time_for_day;

		++iter;
		if( iter != time_list.end() )
		{
			auto next_time_entry = *iter;
			row_data.next_start = next_time_entry.start;
			row_data.first_in_day = is_on_different_days(row_data.prev_start, row_data.start);
			row_data.last_in_day = is_on_different_days(row_data.start, row_data.next_start);
			if(row_data.last_in_day)
			{
				cumulative_time_for_day = 0;
			}
		}
		else
		{
			row_data.first_in_day = is_on_different_days(row_data.prev_start, row_data.start);
			row_data.last_in_day = true;
			row_data.next_start = 0;
		}
		data_rows.push_back(row_data);
	}
	return data_rows;
}

void Details::populate(list<Row_data> data_rows)
{
	tree_model->clear();

	for (auto row_data: data_rows )
	{
		auto tree_iter = tree_model->append();
		TreeModel::Row row = *tree_iter;

		update_row(row, row_data );

	}
}

void Details::update(list<Row_data> data_rows)
{

	for (auto row_data: data_rows )
	{
		//auto treeIter = m_treeModel->append();
		auto row = find_row(row_data.time_id);
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
	auto selected_id = get_selected_id();
	if (selected_id > 0)
	{
		edit_time_entry(selected_id);
	}
}



void Details::edit_time_entry(Time_id id)
{
	if(id==0)
	{
		return;
	}
	auto time_entry = time_accessor.by_id(id);
	if(time_entry.has_value())
	{
		auto dialog = make_shared<gui::Edit_time>(*time_entry, database);
		window_manager.manage_lifespan(dialog);
		dialog->show();
	}
}

}
